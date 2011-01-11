#ifndef __CORE_COMMANDQUEUE_H__
#define __CORE_COMMANDQUEUE_H__

namespace Core
{

class CCommand
{
public:
	virtual void Exec() = 0;
};

template <class Param>
class CCommandParam
{
public:
	virtual void Exec(const Param& _kParam) = 0;
};

template <size_t MaxCmdSize, size_t MaxCount>
class CCommandQueue
{
	
public:

	CCommandQueue()
	: m_iStartIndex(0)
	, m_iEndIndex(0)
	{
	}
	
	template <class CmdType>
	CmdType* Add()
	{
		if (sizeof(CmdType) >= MaxCmdSize)
			return NULL;
		Cmd& kCmd = m_akCmds[m_iEndIndex++];
		m_iEndIndex = m_iEndIndex & (MaxCount - 1);
		CmdType* pkResult = new (kCmd.Buffer) CmdType;
		return pkResult;
	}
	
	void Run()
	{
		while (m_iStartIndex != m_iEndIndex)
		{
			Cmd& kCmd = m_akCmds[m_iStartIndex++];
			m_iStartIndex = m_iStartIndex & (MaxCount - 1);
			CCommand* pkCmd = (CCommand*)kCmd.Buffer;
			pkCmd->Exec();
		}
	}

	template <class Param>
	void Run(const Param& _kParam)
	{
		while (m_iStartIndex != m_iEndIndex)
		{
			Cmd& kCmd = m_akCmds[m_iStartIndex++];
			m_iStartIndex = m_iStartIndex & (MaxCount - 1);
			CCommandParam<Param>* pkCmd = (CCommandParam<Param>*)kCmd.Buffer;
			pkCmd->Exec(_kParam);
		}
	}

protected:

	struct Cmd
	{
		unsigned char Buffer[MaxCmdSize];
	};
	
	Cmd m_akCmds[MaxCount];
	size_t m_iStartIndex;
	size_t m_iEndIndex;

};

}

#endif
