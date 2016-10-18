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

void NoeudSeqInst::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    for (unsigned int i = 0; i < m_instructions.size(); i++)
    {
        auto& type = typeid(*m_instructions[i]);
        
        m_instructions[i]->traduitEnJava(cout, indentation); // on exécute chaque instruction de la séquence    
        if (type != typeid(NoeudInstPour) && type != typeid(NoeudInstSi) && type != typeid(NoeudInstTantQue))
        {
            cout << ";";
        }
        cout << "\n";
    }
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

void NoeudAffectation::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    m_variable->traduitEnJava(cout, indentation);
    cout << " = ";
    m_expression->traduitEnJava(cout, 0);
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

void NoeudOperateurBinaire::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    m_operandeGauche->traduitEnJava(cout, 0);
    cout << " " << m_operateur.getChaine() << " ";
    m_operandeDroit->traduitEnJava(cout, 0);
}



////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(vector<Noeud*>& conditions, vector<Noeud*>& sequences, Noeud* seqElse)
: m_conditions(conditions), m_sequences(sequences), m_seqElse(seqElse) {
}

int NoeudInstSi::executer() 
{
    int i = 0;
    while (i < m_conditions.size() && !m_conditions[i]->executer())
    {
        i++;
    }
    
    if (i != m_conditions.size())
    {
        m_sequences[i]->executer();
    }
    else if (m_seqElse != nullptr)
    {
        m_seqElse->executer();
    }
    
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    for (int i = 0; i < m_conditions.size(); i++)
    {
        if (i == 0)
        {
            cout << setw(indentation*4) << "" << "if(";
        }
        else
        {
            cout << setw(indentation*4) << "" << "else if(";
        }
        m_conditions[i]->traduitEnJava(cout, indentation);
        cout << ")" << "\n";
        cout << setw(indentation*4) << "" << "{\n";
        m_sequences[i]->traduitEnJava(cout, indentation+1);
        cout << setw(indentation*4) << "" << "}\n";
    }
    
    // FAIRE LE ELSE
    cout << setw(indentation*4) << "" << "else\n";
    cout << setw(indentation*4) << "" << "{\n";
    m_seqElse->traduitEnJava(cout, indentation+1);
    cout << setw(indentation*4) << "" << "}";
    
}


NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    cout << setw(indentation*4) << "" << "while" << "(";
    m_condition->traduitEnJava(cout, indentation);
    cout << ")" << "\n";
    cout << setw(indentation*4) << "" << "{\n";
    m_sequence->traduitEnJava(cout, indentation+1);
    cout << setw(indentation*4) << "" << "}";
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

void NoeudInstRepeter::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    cout << setw(indentation*4) << "" << "do\n";
    cout << setw(indentation*4) << "" << "{\n";
    m_sequence->traduitEnJava(cout, indentation+1);
    cout << setw(indentation*4) << "" << "}" << " while" << "(";
    m_condition->traduitEnJava(cout, indentation);
    cout << ")";
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

void NoeudInstPour::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    cout << setw(indentation*4) << "" << "for(";
    // Test de l'initialisation
    if (m_affectation != nullptr)
    {
        m_affectation->traduitEnJava(cout, 0);
    }
    cout << ";";
    
    m_condition->traduitEnJava(cout, 0);
    cout << ";";
    
    if (m_affectation2 != nullptr)
    {
        m_affectation2->traduitEnJava(cout, 0);
    }
    
    cout << ")\n";
    cout << setw(indentation*4) << "" << "{\n";
    m_sequence->traduitEnJava(cout, indentation+1);
    cout << setw(indentation*4) << "" << "}";
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

void NoeudInstEcrire::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    cout << setw(indentation*4) << "" << "System.out.println(";
    
    
    for (int i = 0; i < m_expressions.size(); i++)
    {
        m_expressions[i]->traduitEnJava(cout, 0);
        if (i != m_expressions.size()-1)
        {
            cout << " + ";
        }
    }
    
    cout << ")";
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

void NoeudInstLire::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    for (int i = 0; i < m_expressions.size(); i++)
    {
        cout << setw(indentation*4) << "" << ((SymboleValue*)m_expressions[i])->getChaine() << " = " << "new Scanner(System.in).nextInt()";
        
        if (i != m_expressions.size()-1)
        {
            cout << ";\n";
        }
    }
}
