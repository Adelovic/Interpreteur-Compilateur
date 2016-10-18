#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) : m_lecteur(fichier), m_table(), m_arbre(nullptr) {}

int Interpreteur::getNombreErreur() const
{
    return this->m_nombreErreur;
}

void Interpreteur::addErreur()
{
    this->m_nombreErreur++;
}

void Interpreteur::traduitEnJava(ostream& cout, unsigned int indentation) const
{
    cout << setw(indentation*4) << "" << "public static void main(String[] args)\n";
    cout << setw(indentation*4) << "{\n";
    for (int i = 0; i < this->getTable().getTaille(); i++)
    {
        if (this->getTable()[i] == "<VARIABLE>")
        {
            cout << setw((indentation+1)*4) << "" << "int " << this->getTable()[i].getChaine() << ";\n"; 
        }
    }
    this->getArbre()->traduitEnJava(cout, indentation+1);
    cout << setw(indentation*4) << "}\n";
}
void Interpreteur::analyse() 
{
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException)
{
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) 
  {
    sprintf(messageWhat, "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s", m_lecteur.getLigne(), m_lecteur.getColonne(), symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) 
{
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);            
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) 
{
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat, "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s", m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() 
{
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() 
{
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do 
  {
    sequence->ajoute(inst());
  } 
  while (m_lecteur.getSymbole() == "<VARIABLE>" || 
          m_lecteur.getSymbole() == "si" || 
          m_lecteur.getSymbole() == "tantque" || 
          m_lecteur.getSymbole() == "repeter" || 
          m_lecteur.getSymbole() == "pour" || 
          m_lecteur.getSymbole() == "ecrire" ||
          m_lecteur.getSymbole() == "lire");
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() 
{
  // <inst> ::= <affectation>  ; | <instSi>
  if (m_lecteur.getSymbole() == "<VARIABLE>") 
  {
    Noeud *affect = affectation();
    testerEtAvancer(";");
    return affect;
  }
  else if (m_lecteur.getSymbole() == "si")
    return instSi();
  else if (m_lecteur.getSymbole() == "tantque")
      return instTantQue();
  else if (m_lecteur.getSymbole() == "repeter")
      return instRepeter();
  else if (m_lecteur.getSymbole() == "pour")
      return instPour();
  else if (m_lecteur.getSymbole() == "ecrire")
      return instEcrire();
  else if (m_lecteur.getSymbole() == "lire")
      return instLire();
  // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
  else erreur("Instruction incorrecte");
}

Noeud* Interpreteur::affectation() 
{
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table eton la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() 
{
  // <expression> ::= <facteur> { <opBinaire> <facteur> }  ||  <expression> ::= <terme> {+ <terme> |-<terme> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) 
  {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::terme()
{
    Noeud* fact = facteur();
    while (m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/" )
    {
        Symbole operateur = m_lecteur.getSymbole();
        m_lecteur.avancer();
        Noeud* factDroit = facteur();
        fact = new NoeudOperateurBinaire(operateur, fact, factDroit);
        
    }
    return fact;
    
}

Noeud* Interpreteur::facteur() 
{
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") 
  {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") 
  { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } 
  else if (m_lecteur.getSymbole() == "non")
  { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  }
  else if (m_lecteur.getSymbole() == "(") 
  { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } 
  else
    erreur("Facteur incorrect");
  return fact;
}


Noeud*  expBool();     //     <expBool> ::= <relationET> {ou <relationEt> }
Noeud*  relationEt();   //  <relationEt>::= <relation> {et <relation> }
Noeud*  relation();     //    <relation>::= <expression>{ <opRel> <expression> }

Noeud* Interpreteur::instSi() 
{
    // <instSi> ::= si ( <expression> ) <seqInst> finsi
    // Erreur dans le si initial
    try
    {
        testerEtAvancer("si");
        testerEtAvancer("(");

        vector<Noeud*> conditions;
        vector<Noeud*> sequences;
        conditions.push_back(expression()); // On mémorise la condition
        testerEtAvancer(")");
        sequences.push_back(seqInst());     // On mémorise la séquence d'instruction      
        
        
        while (m_lecteur.getSymbole() == "sinonsi")
        {
            m_lecteur.avancer();
            testerEtAvancer("(");
            conditions.push_back(expression());
            testerEtAvancer(")");
            sequences.push_back(seqInst());
        }

        Noeud* seqElse;
        if (m_lecteur.getSymbole() == "sinon")
        {
            m_lecteur.avancer();
            seqElse = seqInst();
        }

        testerEtAvancer("finsi");

        return new NoeudInstSi(conditions, sequences, seqElse); // Et on renvoie un noeud Instruction Si
    }
    // On va jusqu'au finsi
    catch (SyntaxeException ex)
    {
        cout << "Erreur dans l'instruction si à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        
        while (m_lecteur.getSymbole() != "finsi")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}

Noeud* Interpreteur::instTantQue()
{
    try 
    {      
        testerEtAvancer("tantque");
        testerEtAvancer("(");
        Noeud* condition = expression();
        testerEtAvancer(")");
        Noeud* sequence = seqInst();
        testerEtAvancer("fintantque");
        return new NoeudInstTantQue(condition, sequence);
    }
    catch (SyntaxeException ex)
    {
       cout << "Erreur dans l'instruction tantque à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole() != "fintantque")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}

Noeud* Interpreteur::instRepeter()
{
    Noeud* sequence;
    Noeud* condition;
    try
    {
        testerEtAvancer("repeter");
        sequence = seqInst();
    }
    catch (SyntaxeException ex)
    {
       cout << "Erreur dans l'instruction repeter à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole() != "jusqua")
        {
            m_lecteur.avancer();
        }
        
        while (m_lecteur.getSymbole().getChaine() != ")")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
    
    try
    {
        testerEtAvancer("jusqua");
        testerEtAvancer("(");
        condition = expression();
        testerEtAvancer(")");
        return new NoeudInstRepeter(condition, sequence);   
    }
    catch (SyntaxeException ex)
    {
       cout << "Erreur dans l'instruction repeter à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole().getChaine() != ")")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}

Noeud* Interpreteur::instPour()
{
    // <instPour>    ::= pour ( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
    try
    {
        testerEtAvancer("pour");
        testerEtAvancer("(");
        Noeud* affect = nullptr;
        if (m_lecteur.getSymbole() == "<VARIABLE>")
        {
            affect = affectation();
        }

        testerEtAvancer(";");   

        Noeud* condition = expression();
        testerEtAvancer(";");

        Noeud* affect2 = nullptr;
        if (m_lecteur.getSymbole() == "<VARIABLE>")
        {
            affect2 = affectation();
        }

        testerEtAvancer(")");
        Noeud* sequence = seqInst();
        testerEtAvancer("finpour");

        return new NoeudInstPour(condition, sequence, affect, affect2);
    }
    catch (SyntaxeException ex)
    {
        cout << "Erreur dans l'instruction pour à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole() != "finpour")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}

Noeud* Interpreteur::instEcrire() //  <instEcrire>  ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine> })
{
    try
    {
        testerEtAvancer("ecrire");
        testerEtAvancer("(");

        vector<Noeud*> expressions;

        if (m_lecteur.getSymbole() == "<CHAINE>")
        {
            Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
            expressions.push_back(var);
            m_lecteur.avancer();
        }
        else
        {
            expressions.push_back(expression()); 
        }

        while (m_lecteur.getSymbole().getChaine() == ",")
        {  
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() == "<CHAINE>")
            {

                Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
                expressions.push_back(var);
                m_lecteur.avancer();
            }
            else
            {
                expressions.push_back(expression()); 
            }
        }


        testerEtAvancer(")");
        testerEtAvancer(";");

        return new NoeudInstEcrire(expressions);
    }
    catch (SyntaxeException ex)
    {
        cout << "Erreur dans l'instruction ecrire à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole().getChaine() != ";")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}

Noeud* Interpreteur::instLire()
{   
    try
    {
        testerEtAvancer("lire");
        testerEtAvancer("(");

        vector<Noeud*> expressions;


        if (m_lecteur.getSymbole() == "<VARIABLE>")
        {
            Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
            expressions.push_back(var);
            m_lecteur.avancer();
        }
        else
        {
            erreur("Instruction incorrecte");
        }


        while (m_lecteur.getSymbole().getChaine() == ",")
        {
            m_lecteur.avancer();
            if (m_lecteur.getSymbole() == "<VARIABLE>")
            {
                Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole());
                expressions.push_back(var);
                m_lecteur.avancer();
            }
            else
            {
                erreur("Instruction incorrecte");
            }
        }


        testerEtAvancer(")");
        testerEtAvancer(";");
        return new NoeudInstLire(expressions);
    }
    catch (SyntaxeException ex)
    {
        cout << "Erreur dans l'instruction lire à la ligne " << m_lecteur.getLigne() << " et colonne " << m_lecteur.getColonne() << endl;
        this->addErreur();
        while (m_lecteur.getSymbole().getChaine() != ";")
        {
            m_lecteur.avancer();
        }
        m_lecteur.avancer();
        return nullptr;
    }
}
