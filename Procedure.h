#ifndef PROCEDURE_H
#define PROCEDURE_H

#include <iostream>
#include <stdlib.h>
#include "ArbreAbstrait.h"

class Procedure
{
    public:
        Procedure(std::string name, int parameters, Noeud* arbre); // Construit un symbole valué à partir d'un symbole existant s
	~Procedure( ) {}
    private:
        std::string m_name;
        int m_parameters;
        Noeud* m_arbre;    // L'arbre abstrait
};

#endif /* PROCEDURE_H */

