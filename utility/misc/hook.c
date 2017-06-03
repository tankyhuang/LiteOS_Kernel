
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <hook.h>
#include <los_memory.h>
#include <debug.h>

//	CONSTANT DEFINITIONS


//	TYPE DEFINITIONS

typedef struct HOOKITEM
{
	struct HOOKITEM	*pPrev;
	struct HOOKITEM	*pNext;
	const char		*pSignature;
	PHOOKPROC		pProcedure;
	void			*pUserInstance;
} HOOKITEM, *PHOOKITEM;

typedef struct HOOK
{
	const char	*pSignature;
	PHOOKITEM	pItemList;
	bool		IsExecutingProcedures;
	bool		HasItemDeleted;
} HOOK, *PHOOK;


//	VARIABLE DEFINITIONS

static const char	*m_pHookSignature = "Hook";
static const char	*m_pHookProcSignature = "HookProc";


//	FUNCTION DECLARATIONS

static void DeleteHookItem(PHOOK pHook, PHOOKITEM pItem);


//	FUNCTION DEFINITIONS

HHOOK CreateHook(void)
{
	PHOOK	phook;

	phook = (PHOOK)LOS_MemAlloc(m_aucSysMem0, sizeof(HOOK));
	ASSERT(phook != NULL);

	phook->pSignature = m_pHookSignature;
	phook->pItemList = NULL;
	phook->IsExecutingProcedures = false;
	phook->HasItemDeleted = false;

	return (HHOOK)phook;
}


void DestroyHook(HHOOK hHook)
{
	PHOOK	phook = (PHOOK)hHook;

	ASSERT(phook->pSignature == m_pHookSignature);
	ASSERT(!phook->IsExecutingProcedures);

	while (phook->pItemList != NULL)
	{
		RemoveHookProcedure(hHook, (HHOOKPROC)phook->pItemList);
	}

	phook->pSignature = NULL;
	LOS_MemFree(m_aucSysMem0,phook);
}


HHOOKPROC AddHookProcedure(HHOOK hHook, PHOOKPROC pProcedure, void *pUserInstance)
{
	PHOOK		phook = (PHOOK)hHook;
	PHOOKITEM	pitem;

	ASSERT(phook->pSignature == m_pHookSignature);

	pitem = (PHOOKITEM)LOS_MemAlloc(m_aucSysMem0,sizeof(HOOKITEM));
	ASSERT(pitem != NULL);

	pitem->pSignature		= m_pHookProcSignature;
	pitem->pProcedure		= pProcedure;
	pitem->pUserInstance	= pUserInstance;

	pitem->pPrev = NULL;
	pitem->pNext = phook->pItemList;

	if(phook->pItemList != NULL)
	{
		phook->pItemList->pPrev = pitem;
	}

	phook->pItemList = pitem;

	return (HHOOKPROC)pitem;
}


void RemoveHookProcedure(HHOOK hHook, HHOOKPROC hHookProc)
{
	PHOOK		phook = (PHOOK)hHook;
	PHOOKITEM	pitem = (PHOOKITEM)hHookProc;

	ASSERT(phook->pSignature == m_pHookSignature);
	ASSERT(pitem->pSignature == m_pHookProcSignature);

	pitem->pSignature = NULL;

	if (!phook->IsExecutingProcedures)
	{
		DeleteHookItem(phook, pitem);
	}
	else
	{
		phook->HasItemDeleted = true; // HookProcedure実行中にRemoveが実行された.
	}
}


void ExecuteHookProcedure(HHOOK hHook, ...)
{
	PHOOK		phook = (PHOOK)hHook;
	PHOOKITEM	pitem;
	va_list		args;
	int32_t			a, b, c, d;

	ASSERT(phook->pSignature == m_pHookSignature);

	phook->IsExecutingProcedures = true;

	va_start(args, hHook);
	a = va_arg(args, int32_t);
	b = va_arg(args, int32_t);
	c = va_arg(args, int32_t);
	d = va_arg(args, int32_t);
	//e = va_arg(args, int32_t);
	//f = va_arg(args, int32_t);
	//g = va_arg(args, int32_t);
	//h = va_arg(args, int32_t);

	pitem = phook->pItemList;

	while (pitem != NULL)
	{
		if (pitem->pSignature == m_pHookProcSignature)
		{
			(*pitem->pProcedure)(pitem->pUserInstance, a, b, c, d);
		}
		pitem = pitem->pNext;
	}

	va_end(args);

	phook->IsExecutingProcedures = false;

	if (phook->HasItemDeleted) // HookProcedure実行中にRemoveが実行された.
	{
		PHOOKITEM	pnext_item;

		pitem = phook->pItemList;

		while (pitem != NULL)
		{
			pnext_item = pitem->pNext;

			if (pitem->pSignature != m_pHookProcSignature)
			{
				DeleteHookItem(phook, pitem);
			}

			pitem = pnext_item;
		}

		phook->HasItemDeleted = false;
	}
}


static void DeleteHookItem(PHOOK pHook, PHOOKITEM pItem)
{
	if (pItem->pPrev != NULL)
	{
		pItem->pPrev->pNext = pItem->pNext;
	}
	else
	{
		pHook->pItemList = pItem->pNext;
	}

	if (pItem->pNext != NULL)
	{
		pItem->pNext->pPrev = pItem->pPrev;
	}

	LOS_MemFree(m_aucSysMem0, pItem);
}
