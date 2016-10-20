
#include "Procedure.h"
#include "ArbreAbstrait.h"

    
Procedure::Procedure(std::string name, int parameters) : m_name(name), m_parameters(parameters), m_seqInst(seqInst) {}

int Procedure::executer()
{
    this->m_seqInst->executer();
    
    return 0;
}
