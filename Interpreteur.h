#ifndef INTERPRETEUR_H
#define INTERPRETEUR_H

#include "Symbole.h"
#include "Lecteur.h"
#include "Exceptions.h"
#include "TableSymboles.h"
#include "ArbreAbstrait.h"

class Interpreteur {
public:
    Interpreteur(ifstream & fichier);   // Construit un interpréteur pour interpreter
	                                    //  le programme dans  fichier 
                                      
    void analyse();                     // Si le contenu du fichier est conforme à la grammaire,
	                                    //   cette méthode se termine normalement et affiche un message "Syntaxe correcte".
                                      //   la table des symboles (ts) et l'arbre abstrait (arbre) auront été construits
	                                    // Sinon, une exception sera levée

    inline const TableSymboles & getTable () const  { return m_table;    } // accesseur	
    inline Noeud* getArbre () const { return m_arbre; }                    // accesseur
    void traduitEnCpp(ostream& cout, unsigned int indentation) const;
        
        
    int getNombreErreur() const;
    void addErreur();
	
private:
    Lecteur        m_lecteur;  // Le lecteur de symboles utilisé pour analyser le fichier
    TableSymboles  m_table;    // La table des symboles valués
    Noeud*         m_arbre;    // L'arbre abstrait

    // Implémentation de la grammaire
    Noeud*  programme();   //   <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
    Noeud*  seqInst();	   //     <seqInst> ::= <inst> { <inst> }
    Noeud*  inst();	   //        <inst> ::= <affectation> ; | <instSi>
    Noeud*  affectation(); // <affectation> ::= <variable> = <expression> 
    
    Noeud*  expression();  //  <expression> ::= <expression> ::= <terme> {+ <terme> |-<terme> }
    Noeud*  terme();       //       <terme> ::=  <facteur> { * <facteur> | /<facteur> } 
    Noeud*  facteur();     
    
    Noeud* expBool();
    Noeud* relationEt();
    Noeud* relation();
    
    Noeud*  instSi();      //      <instSi> ::= si ( <expression> ) <seqInst> finsi
    Noeud*  instTantQue(); // <instTantQue> ::= tantque ( <expression> ) <seqInst> tantque
    Noeud*  instRepeter(); // <instRepeter> ::= repeter <seqInst> jusqua ( <expression> )
    Noeud*  instPour(); //    <instPour>    ::= pour ( [ <affectation> ] ; <expression> ;[ <affectation> ]) <seqInst> finpour
    Noeud*  instEcrire(); //  <instEcrire>  ::= ecrire ( <expression> | <chaine> {, <expression> | <chaine> })
    Noeud*  instLire(); //    <instLire>    ::= lire( <variable> {, <variable> })
    Noeud*  instSelon(); // <instSelon>     ::= selon(<variable>) <case> { <case> } {defaut: <seqInst>}
                            // <case>        ::= cas <entier>: <seqInst>
    
    // outils pour simplifier l'analyse syntaxique
    void tester (const string & symboleAttendu) const throw (SyntaxeException);   // Si symbole courant != symboleAttendu, on lève une exception
    void testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException); // Si symbole courant != symboleAttendu, on lève une exception, sinon on avance
    void erreur (const string & mess) const throw (SyntaxeException);             // Lève une exception "contenant" le message mess
    
    int m_nombreErreur = 0;
};

#endif /* INTERPRETEUR_H */
