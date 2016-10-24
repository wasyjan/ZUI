﻿#include <ZUI.h>




void* CALLBACK ZuiHorizontalLayoutProc(int ProcId, ZuiControl cp, ZuiHorizontalLayout p, void* Param1, void* Param2, void* Param3) {
	switch (ProcId)
	{
	case Proc_CoreInit:
		return 0;
	case Proc_OnCreate: {
		p = (ZuiHorizontalLayout)malloc(sizeof(ZHorizontalLayout));
		memset(p, 0, sizeof(ZHorizontalLayout));
		//创建继承的控件 保存数据指针
		p->old_udata = ZuiLayoutProc(Proc_OnCreate, cp, 0, 0, 0, 0);
		p->old_call = (ZCtlProc)&ZuiLayoutProc;

		return p;
		break;
	}
	case Proc_SetPos: {
		ZuiDefaultControlProc(ProcId, cp, 0, Param1, Param2, Param3);
		RECT rc = cp->m_rcItem;
		ZuiLayout op = (ZuiLayout)p->old_udata;
		// Adjust for inset
		rc.left += op->m_rcInset.left;
		rc.top += op->m_rcInset.top;
		rc.right -= op->m_rcInset.right;
		rc.bottom -= op->m_rcInset.bottom;
		if (darray_len(op->m_items) == 0) {
			return 0;
		}

		// Determine the minimum size
		SIZE szAvailable = { rc.right - rc.left, rc.bottom - rc.top };

		int nAdjustables = 0;
		int cxFixed = 0;
		int nEstimateNum = 0;
		for (int it1 = 0; it1 < darray_len(op->m_items); it1++) {
			ZuiControl pControl = (ZuiControl)(op->m_items->data[it1]);
			if (!pControl->m_bVisible) continue;
			if (pControl->m_bFloat) continue;
			SIZE sz;
			SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szAvailable, 0, 0);
			sz.cx = psz->cx;
			sz.cy = psz->cy;
			if (sz.cx == 0) {
				nAdjustables++;
			}
			else {
				if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
				if (sz.cx >(LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
			}
			cxFixed += sz.cx + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->left + ((RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0)))->right;
			nEstimateNum++;
		}
		cxFixed += (nEstimateNum - 1) * op->m_iChildPadding;

		// Place elements
		int cxNeeded = 0;
		int cxExpand = 0;
		if (nAdjustables > 0) cxExpand = MAX(0, (szAvailable.cx - cxFixed) / nAdjustables);
		// Position the elements
		SIZE szRemaining = szAvailable;

		int iPosX = rc.left;
		int iAdjustable = 0;
		int cxFixedRemaining = cxFixed;
		for (int it2 = 0; it2 < darray_len(op->m_items); it2++) {
			ZuiControl pControl = (ZuiControl)(op->m_items->data[it2]);
			if (!pControl->m_bVisible) continue;
			if (pControl->m_bFloat) {
				ZuiControlCall(Proc_Layout_SetFloatPos, cp, (void *)it2, 0, 0);
				continue;
			}

			RECT *rcPadding = (RECT *)(ZuiControlCall(Proc_GetPadding, pControl, 0, 0, 0));
			szRemaining.cx -= rcPadding->left;
			SIZE sz;
			SIZE * psz = (SIZE *)ZuiControlCall(Proc_EstimateSize, pControl, (void *)&szRemaining, 0, 0);
			sz.cx = psz->cx;
			sz.cy = psz->cy;
			if (sz.cx == 0) {
				iAdjustable++;
				sz.cx = cxExpand;
				// Distribute remaining to last element (usually round-off left-overs)
				if (iAdjustable == nAdjustables) {
					sz.cx = MAX(0, szRemaining.cx - rcPadding->right - cxFixedRemaining);
				}
				if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
				if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
			}
			else {
				if (sz.cx < (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMinWidth, pControl, 0, 0, 0);
				if (sz.cx > (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0)) sz.cx = (LONG)ZuiControlCall(Proc_GetMaxWidth, pControl, 0, 0, 0);
				cxFixedRemaining -= sz.cx + rcPadding->left + rcPadding->right;
			}
			cxFixedRemaining -= op->m_iChildPadding;

			sz.cy = (LONG)ZuiControlCall(Proc_GetFixedHeight, pControl, 0, 0, 0);
			if (sz.cy == 0) sz.cy = rc.bottom - rc.top - rcPadding->top - rcPadding->bottom;
			if (sz.cy < 0) sz.cy = 0;
			if (sz.cy < (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMinHeight, pControl, 0, 0, 0);
			if (sz.cy > (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0)) sz.cy = (LONG)ZuiControlCall(Proc_GetMaxHeight, pControl, 0, 0, 0);
			RECT rcCtrl = { iPosX + rcPadding->left, rc.top + rcPadding->top, iPosX + sz.cx + rcPadding->left , rc.top + rcPadding->top + sz.cy };
			ZuiControlCall(Proc_SetPos, pControl, &rcCtrl, FALSE, 0);

			iPosX += sz.cx + op->m_iChildPadding + rcPadding->left + rcPadding->right;
			cxNeeded += sz.cx + rcPadding->left + rcPadding->right;
			szRemaining.cx -= sz.cx + op->m_iChildPadding + rcPadding->right;
		}
		cxNeeded += (nEstimateNum - 1) * op->m_iChildPadding;

		return 0;
		break;
	}
	case Proc_OnPaint: {


		break;
	}
	default:
		break;
	}
	return p->old_call(ProcId, cp, p->old_udata, Param1, Param2, Param3);
}
