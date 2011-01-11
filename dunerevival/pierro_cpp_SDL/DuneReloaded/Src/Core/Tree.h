
#ifndef __CORE_TREE_H__
#define __CORE_TREE_H__

namespace Core
{

struct CTreeVisitorContext
{
	
};

template <class NodeClass>
class TreeNode
{

public:

	class CVisitor
	{

	public:

		virtual ~CVisitor() {};
		virtual void PreVisit(NodeClass* _pkNode, const CTreeVisitorContext& _kCtx) const = 0;
		virtual void PostVisit(NodeClass* _pkNode, const CTreeVisitorContext& _kCtx) const = 0;

	};

	TreeNode()
	: m_pkNext(0)
	, m_pkPrevious(0)
	, m_pkParent(0)
	, m_pkFirstChild(0)
	, m_bOwned(true)
	{
		m_pkPrevious=static_cast<NodeClass*>(this);
		m_pkNext=static_cast<NodeClass*>(this);
	}

	virtual ~TreeNode()
	{
		if (m_pkParent)
		{
			m_pkParent->DetachChild(static_cast<NodeClass*>(this));
		}
		while (m_pkFirstChild)
		{
			TreeNode<NodeClass> *pkTmp=m_pkFirstChild;
			DetachChild(m_pkFirstChild);
			if (!pkTmp->m_bOwned)
				delete(pkTmp);
		}
	}

	inline NodeClass *GetParent()
	{
		return(m_pkParent);
	}

	int ComputeChildCount()
	{
		int iCount = 0;
		NodeClass *pkCurrent = 0;
		while ((pkCurrent=Iterate(pkCurrent)) != 0)
			++iCount;
		return(iCount);
	}

	virtual void AttachChild(NodeClass *_pkNode)
	{
		if (_pkNode == 0)
			return;

		if (_pkNode->m_pkParent != 0)
			return;

		_pkNode->m_pkParent=static_cast<NodeClass*>(this);

		if (m_pkFirstChild == 0)
		{
			m_pkFirstChild=_pkNode;
			return;
		}

		_pkNode->m_pkPrevious=m_pkFirstChild->m_pkPrevious;
		_pkNode->m_pkNext=m_pkFirstChild;
		m_pkFirstChild->m_pkPrevious->m_pkNext=_pkNode;
		m_pkFirstChild->m_pkPrevious=_pkNode;
	}

	void DetachChild(NodeClass *_pkNode)
	{
		if (_pkNode==0)
			return;

		if (_pkNode->m_pkParent!=this)
			return;

		_pkNode->m_pkParent=0;
		if (_pkNode->m_pkPrevious==_pkNode)
		{
			m_pkFirstChild=0;
		}
		else
		{
			if (_pkNode==m_pkFirstChild)
				m_pkFirstChild=_pkNode->m_pkNext;
			_pkNode->m_pkPrevious->m_pkNext=_pkNode->m_pkNext;
			_pkNode->m_pkNext->m_pkPrevious=_pkNode->m_pkPrevious;
		}
		_pkNode->m_pkNext=_pkNode;
		_pkNode->m_pkPrevious=_pkNode;
	}

	NodeClass *Iterate(NodeClass *_pkCurrentNode)
	{
		if (_pkCurrentNode==0)
			return(m_pkFirstChild);
		NodeClass *pkNext=_pkCurrentNode->m_pkNext;
		if (pkNext==m_pkFirstChild)
			return(0);
		return(pkNext);
	}

	void Visit(const CVisitor& _kVisitor)
	{
		NodeClass *pkThis=static_cast<NodeClass*>(this);

		_kVisitor.PreVisit(pkThis, CTreeVisitorContext());

		NodeClass *pkNode=0;
		int iChildIndex=0;
		while ((pkNode=Iterate(pkNode))!=0)
			pkNode->Visit(_kVisitor);

		_kVisitor.PostVisit(pkThis, CTreeVisitorContext());
	}
	
	void SetOwned(bool _bOwned) { m_bOwned=_bOwned; }

protected:

	NodeClass *m_pkNext;
	NodeClass *m_pkPrevious;
	NodeClass *m_pkParent;
	NodeClass *m_pkFirstChild;

	bool m_bOwned;

};

}

#endif

