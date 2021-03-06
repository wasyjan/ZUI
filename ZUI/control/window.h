﻿/**
* @file		Windows.h
* @brief	窗体控件实现.
* @author	[Zero](22249030)
* @version	1.0
* @date		$date
* @par History:
*
* [2016-10-25] <Zero> v1.0
*
* + v1.0版发布.
*
*/
#ifndef __UIWINDOW_H__
#define __UIWINDOW_H__

#define	Proc_Window_SetNoBox	1001	//设置为无边框窗体
/**窗口控件结构*/
typedef struct _ZWindow
{
	ZRect m_rect;
	ZuiBool m_nobox;
	HWND m_hWnd;
	WNDPROC m_OldWndProc;
	ZuiControl root;
	ZuiPaintManager m_pm;
	ZCtlProc old_call;
	void *old_udata;
}*ZuiWindow, ZWindow;
ZEXPORT ZuiAny ZCALL ZuiWindowProc(ZuiInt ProcId, ZuiControl cp, ZuiWindow p, ZuiAny Param1, ZuiAny Param2, ZuiAny Param3);


#endif	//__UIWINDOW_H__