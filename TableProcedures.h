#ifndef TABLESPROCEDURES_H
#define TABLESPROCEDURES_H

#include "Procedure.h"
#include <vector>
#include <iostream>
using namespace std;

class TableProcedures {
public:
    TableProcedures(); // Construit une table vide de pointeurs sur des symboles valués
    void ajoute(const Procedure & procedure);
    // si symbole est identique à un symbole valué déjà présent dans la table,
    // on renvoie un pointeur sur ce symbole valué
    // Sinon on insère un nouveau symbole valué correspondant à symbole
    // et on renvoie un pointeur sur le nouveau symbole valué inséré

    inline unsigned int getTaille() const { return m_procedures.size(); } // Taille de la table des procédures

    inline const Procedure & operator[](unsigned int i) const { return *m_procedures[i]; } // accès au ième procedure de la table
    friend ostream & operator<<(ostream & cout, const TableProcedures & tp); // affiche ts sur cout
    
private:
    vector<Procedure*> m_procedures;
    
};
#endif
