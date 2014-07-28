#ifndef UIMICROBLOG_HPP
#define UIMICROBLOG_HPP

namespace DuiLib
{
class CMicroBlogUI : public CListUI
{
public:
	CMicroBlogUI(CPaintManagerUI& paint_manager);
	~CMicroBlogUI();

private:
	CPaintManagerUI& paint_manager_;
};

} // DuiLib

#endif // UIMICROBLOG_HPP