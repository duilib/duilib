// Module:  Log4CPLUS
// File:    socket-win32.cxx
// Created: 4/2003
// Author:  Tad E. Smith
//
//
// Copyright 2003-2013 Tad E. Smith
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <log4cplus/config.hxx>
#if defined (LOG4CPLUS_USE_WINSOCK)

#include <cassert>
#include <cerrno>
#include <vector>
#include <cstring>
#include <log4cplus/internal/socket.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/thread/threads.h>
#include <log4cplus/helpers/stringhelper.h>


/////////////////////////////////////////////////////////////////////////////
// file LOCAL Classes
/////////////////////////////////////////////////////////////////////////////

namespace
{

enum WSInitStates
{
    WS_UNINITIALIZED,
    WS_INITIALIZING,
    WS_INITIALIZED
};


static WSADATA wsa;
static LONG volatile winsock_state = WS_UNINITIALIZED;


static
void
init_winsock_worker ()
{
    log4cplus::helpers::LogLog * loglog
        = log4cplus::helpers::LogLog::getLogLog ();

    // Try to change the state to WS_INITIALIZING.
    LONG val = ::InterlockedCompareExchange (
        const_cast<LPLONG>(&winsock_state), WS_INITIALIZING, WS_UNINITIALIZED);
    switch (val)
    {
    case WS_UNINITIALIZED:
    {
        int ret = WSAStartup (MAKEWORD (2, 2), &wsa);
        if (ret != 0)
        {
            // Revert the state back to WS_UNINITIALIZED to unblock other
            // threads and let them throw exception.
            val = ::InterlockedCompareExchange (
                const_cast<LPLONG>(&winsock_state), WS_UNINITIALIZED,
                WS_INITIALIZING);
            assert (val == WS_INITIALIZING);
            loglog->error (LOG4CPLUS_TEXT ("Could not initialize WinSock."),
                true);
        }

        // WinSock is initialized, change the state to WS_INITIALIZED.
        val = ::InterlockedCompareExchange (
            const_cast<LPLONG>(&winsock_state), WS_INITIALIZED,
            WS_INITIALIZING);
        assert (val == WS_INITIALIZING);
        return;
    }

    case WS_INITIALIZING:
        // Wait for state change.
        while (true)
        {
            switch (winsock_state)
            {
            case WS_INITIALIZED:
                return;

            case WS_INITIALIZING:
                log4cplus::thread::yield ();
                continue;

            default:
                assert (0);
                loglog->error (LOG4CPLUS_TEXT ("Unknown WinSock state."), true);
            }
        }

    case WS_INITIALIZED:
        // WinSock is already initialized.
        return;

    default:
        assert (0);
        loglog->error (LOG4CPLUS_TEXT ("Unknown WinSock state."), true);
    }
}


static
void
init_winsock ()
{
    // Quick check first to avoid the expensive interlocked compare
    // and exchange.
    if (winsock_state == WS_INITIALIZED)
        return;
    else
        init_winsock_worker ();
}


struct WinSockInitializer
{
    ~WinSockInitializer ()
    {
        if (winsock_state == WS_INITIALIZED)
            WSACleanup ();
    }

    static WinSockInitializer winSockInitializer;
};


WinSockInitializer WinSockInitializer::winSockInitializer;


} // namespace


namespace log4cplus { namespace helpers {


/////////////////////////////////////////////////////////////////////////////
// Global Methods
/////////////////////////////////////////////////////////////////////////////

SOCKET_TYPE
openSocket(unsigned short port, SocketState& state)
{
    struct sockaddr_in server;

    init_winsock ();

    SOCKET sock = WSASocket (AF_INET, SOCK_STREAM, AF_UNSPEC, 0, 0
#if defined (WSA_FLAG_NO_HANDLE_INHERIT)
        , WSA_FLAG_NO_HANDLE_INHERIT
#else
        , 0
#endif
        );

    if (sock == INVALID_OS_SOCKET_VALUE)
        goto error;

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sock, reinterpret_cast<struct sockaddr*>(&server), sizeof(server))
        != 0)
        goto error;

    if (::listen(sock, 10) != 0)
        goto error;

    state = ok;
    return to_log4cplus_socket (sock);

error:
    int eno = WSAGetLastError ();

    if (sock != INVALID_OS_SOCKET_VALUE)
        ::closesocket (sock);

    set_last_socket_error (eno);
    return INVALID_SOCKET_VALUE;
}


SOCKET_TYPE
connectSocket(const tstring& hostn, unsigned short port, bool udp, SocketState& state)
{
    struct hostent * hp;
    struct sockaddr_in insock;
    int retval;

    init_winsock ();

    SOCKET sock = WSASocket (AF_INET, (udp ? SOCK_DGRAM : SOCK_STREAM),
        AF_UNSPEC, 0, 0
#if defined (WSA_FLAG_NO_HANDLE_INHERIT)
        , WSA_FLAG_NO_HANDLE_INHERIT
#else
        , 0
#endif
        );
    if (sock == INVALID_OS_SOCKET_VALUE)
        goto error;

    hp = ::gethostbyname( LOG4CPLUS_TSTRING_TO_STRING(hostn).c_str() );
    if (hp == 0 || hp->h_addrtype != AF_INET)
    {
        insock.sin_family = AF_INET;
        INT insock_size = sizeof (insock);
        INT ret = WSAStringToAddress (const_cast<LPTSTR>(hostn.c_str ()),
            AF_INET, 0, reinterpret_cast<struct sockaddr *>(&insock),
            &insock_size);
        if (ret == SOCKET_ERROR || insock_size != static_cast<INT>(sizeof (insock)))
        {
            state = bad_address;
            goto error;
        }
    }
    else
        std::memcpy (&insock.sin_addr, hp->h_addr_list[0],
            sizeof (insock.sin_addr));

    insock.sin_port = htons(port);
    insock.sin_family = AF_INET;

    while(   (retval = ::connect(sock, (struct sockaddr*)&insock, sizeof(insock))) == -1
          && (WSAGetLastError() == WSAEINTR))
        ;
    if (retval == SOCKET_ERROR)
        goto error;

    state = ok;
    return to_log4cplus_socket (sock);

error:
    int eno = WSAGetLastError ();

    if (sock != INVALID_OS_SOCKET_VALUE)
        ::closesocket (sock);

    set_last_socket_error (eno);
    return INVALID_SOCKET_VALUE;
}


SOCKET_TYPE
acceptSocket(SOCKET_TYPE sock, SocketState & state)
{
    init_winsock ();

    SOCKET connected_socket = ::accept (to_os_socket (sock), NULL, NULL);

    if (connected_socket != INVALID_OS_SOCKET_VALUE)
        state = ok;
    else
        set_last_socket_error (WSAGetLastError ());

    return to_log4cplus_socket (connected_socket);
}



int
closeSocket(SOCKET_TYPE sock)
{
    return ::closesocket (to_os_socket (sock));
}


int
shutdownSocket(SOCKET_TYPE sock)
{
    return ::shutdown (to_os_socket (sock), SD_BOTH);
}


long
read(SOCKET_TYPE sock, SocketBuffer& buffer)
{
    long res, read = 0;
    os_socket_type const osSocket = to_os_socket (sock);

    do
    {
        res = ::recv(osSocket,
                     buffer.getBuffer() + read,
                     static_cast<int>(buffer.getMaxSize() - read),
                     0);
        if (res == SOCKET_ERROR)
        {
            set_last_socket_error (WSAGetLastError ());
            return res;
        }

        // A return of 0 indicates the socket is closed,
        // return to prevent an infinite loop.
        if (res == 0)
            return read;

        read += res;
    }
    while (read < static_cast<long>(buffer.getMaxSize()));

    return read;
}



long
write(SOCKET_TYPE sock, const SocketBuffer& buffer)
{
    long ret = ::send (to_os_socket (sock), buffer.getBuffer(),
        static_cast<int>(buffer.getSize()), 0);
    if (ret == SOCKET_ERROR)
        set_last_socket_error (WSAGetLastError ());
    return ret;
}


long
write(SOCKET_TYPE sock, const std::string & buffer)
{
    long ret = ::send (to_os_socket (sock), buffer.c_str (),
        static_cast<int>(buffer.size ()), 0);
    if (ret == SOCKET_ERROR)
        set_last_socket_error (WSAGetLastError ());
    return ret;
}


tstring
getHostname (bool fqdn)
{
    init_winsock ();

    char const * hostname = "unknown";
    int ret;
    std::vector<char> hn (1024, 0);

    while (true)
    {
        ret = ::gethostname (&hn[0], static_cast<int>(hn.size ()) - 1);
        if (ret == 0)
        {
            hostname = &hn[0];
            break;
        }
        else if (ret != 0 && WSAGetLastError () == WSAEFAULT)
            // Out buffer was too short. Retry with buffer twice the size.
            hn.resize (hn.size () * 2, 0);
        else
            break;
    }

    if (ret != 0 || (ret == 0 && ! fqdn))
        return LOG4CPLUS_STRING_TO_TSTRING (hostname);

    struct ::hostent * hp = ::gethostbyname (hostname);
    if (hp)
        hostname = hp->h_name;

    return LOG4CPLUS_STRING_TO_TSTRING (hostname);
}


int
setTCPNoDelay (SOCKET_TYPE sock, bool val)
{
    int result;
    int enabled = static_cast<int>(val);
    if ((result = setsockopt(sock, IPPROTO_TCP, TCP_NODELAY,
            reinterpret_cast<char*>(&enabled),sizeof(enabled))) != 0)
    {
        int eno = WSAGetLastError ();
        set_last_socket_error (eno);
    }

    return result;
}


//
// ServerSocket OS dependent stuff
//

namespace
{

static
bool
setSocketBlocking (SOCKET_TYPE s)
{
    u_long val = 0;
    int ret = ioctlsocket (to_os_socket (s), FIONBIO, &val);
    if (ret == SOCKET_ERROR)
    {
        set_last_socket_error (WSAGetLastError ());
        return false;
    }
    else
        return true;
}

static
bool
removeSocketEvents (SOCKET_TYPE s, HANDLE ev)
{
    // Clean up socket events handling.

    int ret = WSAEventSelect (to_os_socket (s), ev, 0);
    if (ret == SOCKET_ERROR)
    {
        set_last_socket_error (WSAGetLastError ());
        return false;
    }
    else
        return true;
}


static
bool
socketEventHandlingCleanup (SOCKET_TYPE s, HANDLE ev)
{
    bool ret = removeSocketEvents (s, ev);
    ret = setSocketBlocking (s) && ret;
    ret = WSACloseEvent (ev) && ret;
    return ret;
}


} // namespace


ServerSocket::ServerSocket(unsigned short port)
{
    sock = openSocket (port, state);
    if (sock == INVALID_SOCKET_VALUE)
    {
        err = get_last_socket_error ();
        return;
    }

    HANDLE ev = WSACreateEvent ();
    if (ev == WSA_INVALID_EVENT)
    {
        err = WSAGetLastError ();
        closeSocket (sock);
        sock = INVALID_SOCKET_VALUE;
    }
    else
    {
        assert (sizeof (std::ptrdiff_t) >= sizeof (HANDLE));
        interruptHandles[0] = reinterpret_cast<std::ptrdiff_t>(ev);
    }
}

Socket
ServerSocket::accept ()
{
    int const N_EVENTS = 2;
    HANDLE events[N_EVENTS] = {
        reinterpret_cast<HANDLE>(interruptHandles[0]) };
    HANDLE & accept_ev = events[1];
    int ret;

    // Create event and prime socket to set the event on FD_ACCEPT.

    accept_ev = WSACreateEvent ();
    if (accept_ev == WSA_INVALID_EVENT)
    {
        set_last_socket_error (WSAGetLastError ());
        goto error;
    }

    ret = WSAEventSelect (to_os_socket (sock), accept_ev, FD_ACCEPT);
    if (ret == SOCKET_ERROR)
    {
        set_last_socket_error (WSAGetLastError ());
        goto error;
    }

    do
    {
        // Wait either for interrupt event or actual connection coming in.

        DWORD wsawfme = WSAWaitForMultipleEvents (N_EVENTS, events, FALSE,
            WSA_INFINITE, TRUE);
        switch (wsawfme)
        {
        case WSA_WAIT_TIMEOUT:
        case WSA_WAIT_IO_COMPLETION:
            // Retry after timeout or APC.
            continue;

        // This is interrupt signal/event.
        case WSA_WAIT_EVENT_0:
        {
            // Reset the interrupt event back to non-signalled state.

            ret = WSAResetEvent (reinterpret_cast<HANDLE>(interruptHandles[0]));

            // Clean up socket events handling.

            ret = socketEventHandlingCleanup (sock, accept_ev);

            // Return Socket with state set to accept_interrupted.

            return Socket (INVALID_SOCKET_VALUE, accept_interrupted, 0);
        }

        // This is accept_ev.
        case WSA_WAIT_EVENT_0 + 1:
        {
            // Clean up socket events handling.

            ret = socketEventHandlingCleanup (sock, accept_ev);

            // Finally, call accept().

            SocketState st = not_opened;
            SOCKET_TYPE clientSock = acceptSocket (sock, st);
            int eno = 0;
            if (clientSock == INVALID_SOCKET_VALUE)
                eno = get_last_socket_error ();

            return Socket (clientSock, st, eno);
        }

        case WSA_WAIT_FAILED:
        default:
            set_last_socket_error (WSAGetLastError ());
            goto error;
        }
    }
    while (true);


error:;
    DWORD eno = get_last_socket_error ();

    // Clean up socket events handling.

    if (sock != INVALID_SOCKET_VALUE)
    {
        (void) removeSocketEvents (sock, accept_ev);
        (void) setSocketBlocking (sock);
    }

    if (accept_ev != WSA_INVALID_EVENT)
        WSACloseEvent (accept_ev);

    set_last_socket_error (eno);
    return Socket (INVALID_SOCKET_VALUE, not_opened, eno);
}


void
ServerSocket::interruptAccept ()
{
    (void) WSASetEvent (reinterpret_cast<HANDLE>(interruptHandles[0]));
}


ServerSocket::~ServerSocket()
{
    (void) WSACloseEvent (reinterpret_cast<HANDLE>(interruptHandles[0]));
}



} } // namespace log4cplus { namespace helpers {

#endif // LOG4CPLUS_USE_WINSOCK
