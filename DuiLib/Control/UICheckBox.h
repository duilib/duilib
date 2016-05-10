#ifndef __UICHECKBOX_H__
#define __UICHECKBOX_H__

#pragma once

namespace DuiLib
{
	/// 最普通的单选按钮控件，只有是、否两种结果
	/// 派生于COptionUI，只是每组只有一个按钮而已，组名为空，配置文件默认属性举例：
	/// <CheckBox name="CheckBox" value="height='20' align='left' textpadding='24,0,0,0' normalimage='file='sys_check_btn.png' source='0,0,20,20' dest='0,0,20,20'' selectedimage='file='sys_check_btn.png' source='20,0,40,20' dest='0,0,20,20'' disabledimage='file='sys_check_btn.png' source='40,0,60,20' dest='0,0,20,20''"/>

	class DUILIB_API CCheckBoxUI : public COptionUI
	{
	public:
		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetCheck(bool bCheck, bool bTriggerEvent=true);
		bool GetCheck() const;
	};
}

#endif // __UICHECKBOX_H__
