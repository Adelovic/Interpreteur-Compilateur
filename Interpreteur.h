#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableProcedures.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
    Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
    void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

    inline const TableProcedures & getTable () const  { return m_procedures;    } // accesseur	
    void traduitEnCpp(ostream& cout, unsigned int indentation) const;
        
        
    int getNombreErreur() const;
    void addErreur();
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableProcedures m_procedures;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait

    // Implémentation de la grammaire
    Noeud*  programme(Procedure& procedure);   //  <programme>   ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst(Procedure& procedure);	   //  <seqInst>     ::= <inst> { <inst> }
    Noeud*  inst(Procedure& procedure);	   //  <inst>        ::= <affectation> ; | <instSi>
    Noeud*  affectation(Procedure& procedure); //  <affectation> ::= <variable> = (<expression> 
    
    Noeud*  expression(Procedure& procedure);  //  <expression> ::= <terme> {+ <terme> |-<terme> }
    Noeud*  terme(Procedure& procedure);       //  <terme>      ::= <facteur> { * <facteur> | /<facteur> }      // 
    Noeud*  facteur(Procedure& procedure);     //  <facteur>    ::= <entier> | <variable> | non <expBool> | ( <expBool> )
   
    Noeud* expBool(Procedure& procedure);       // <expBool>    ::= <relationET> { ou <relationEt }
    Noeud* relationEt(Procedure& procedure);    // <relationEt> ::= <relation> { et <relation> }
    Noeud* relation(Procedure& procedure);      // <expression> ::= <expression> { <opRel> <expression> }
    
    
    Noeud*  instSi(Procedure& procedure);      //  <instSi>      ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instTantQue(Procedure& procedure); //  <instTantQue> ::= tantque ( <expression> ) <seqInst> tantque
    Noeud*  instRepeter(Procedure& procedure); //  <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
    Noeud*  instPour(Procedure& procedure); //     <instPour>    ::= pour ( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
    Noeud*  instEcrire(Procedure& procedure); //   <instEcrire>  ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine> })
    Noeud*  instLire(Procedure& procedure); //     <instLire>    ::= lire( <variable> {, <variable> })
    
    Noeud*  instProcedure();
    
    // outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const throw (SyntaxeException);   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const throw (SyntaxeException);             // Lève une exception "contenant" le message mess
    
    int m_nombreErreur = 0;
};

#endif /* INTERPRETEUR_H */
