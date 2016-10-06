#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"
#include <typeinfo>

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}


NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() 
{
    do
    {
        m_sequence->executer();
    }
    while (m_condition->executer());
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affectation, Noeud* affectation2)
: m_condition(condition), m_sequence(sequence), m_affectation(affectation), m_affectation2(affectation2) {
}

int NoeudInstPour::executer() 
{
    if (m_affectation != nullptr)
    {
        m_affectation->executer();
    }
    while (m_condition->executer())    
    {
        m_sequence->executer();
        if (m_affectation2 != nullptr)
        {
            m_affectation2->executer();
        }
    }
    
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> & expressions): m_expressions(expressions) {}

int NoeudInstEcrire::executer() 
{
    for (int i = 0; i < m_expressions.size(); i++)
    {
        if ((typeid(*m_expressions[i])==typeid(SymboleValue)) &&  *((SymboleValue*)m_expressions[i])== "<CHAINE>" ) 
        {
            cout << ((SymboleValue*)m_expressions[i])->getChaine().substr(1, ((SymboleValue*)m_expressions[i])->getChaine().size()-2);
        }
        else
        {
            cout << m_expressions[i]->executer();
        }
    }
    
  return 0; // La valeur renvoyée ne représente rien !
}

NoeudInstLire::NoeudInstLire(vector<Noeud*> & expressions): m_expressions(expressions) {}

int NoeudInstLire::executer() 
{
    for (int i = 0; i < m_expressions.size(); i++)
    {
        int valeur;
        cin >> valeur;
        ((SymboleValue*)m_expressions[i])->setValeur(valeur);
    }
    
    
  return 0; // La valeur renvoyée ne représente rien !
}
