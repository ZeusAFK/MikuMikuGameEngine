#pragma once

// ツリー構造を持たせたいデータのクラスに継承して使ってね

template<class T>
class TreeNode
{
private:
	T* m_parent;
	T* m_child;
	
	T* m_siblingPrev;
	T* m_siblingNext;

protected:
	TreeNode()
		: m_parent(NULL)
		, m_child(NULL)
		, m_siblingPrev(NULL)
		, m_siblingNext(NULL)
	{
	}

public:
	virtual ~TreeNode()
	{
		DetachParent();
		m_parent = NULL;

		T* child = m_child;
		while( child )
		{
			T* next = child->m_siblingNext;

			delete child;

			child = next;
		}
	}

	void SetParent( T* parent )
	{
		DetachParent();

		ChangeParent( parent );
		m_parent = parent;

		if( m_parent )
		{
			if( m_parent->m_child )
			{
				T* child = m_parent->m_child;
				while( child )
				{
					if( !child->m_siblingNext )
					{
						break;
					}

					child = child->m_siblingNext;
				}

				if( child )
				{
					child->m_siblingNext = (T*)this;
					m_siblingPrev = child;
				}
			}
			else
			{
				m_parent->m_child = (T*)this;
			}
		}
	}

	T* GetParent()
	{
		return m_parent;
	}

	T* GetChild()
	{
		return m_child;
	}

	T* GetSiblingPrev()
	{
		return m_siblingPrev;
	}

	T* GetSiblingNext()
	{
		return m_siblingNext;
	}

private:
	void DetachParent()
	{
		if( m_parent )
		{
			if( m_siblingPrev )
			{
				m_siblingPrev->m_siblingNext = m_siblingNext;
			}
			else
			{
				m_parent->m_child = m_siblingNext;
			}

			if( m_siblingNext )
			{
				m_siblingNext->m_siblingPrev = m_siblingPrev;
			}

			m_siblingPrev = NULL;
			m_siblingNext = NULL;
		}
	}

protected:
	virtual void ChangeParent( T* nextParent ){};
};
