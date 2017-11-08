/*
 * Copyright 2012 Johan Eliasson (johane@cs.umu.se). Tillåtelse ges för användning på kurserna
 * i Datastrukturer och algoritmer vid Umeå Universitet. All annan användning kräver författarens
 * tillstånd.
 */

/*
 Implementation av ordnad riktad graf med möjlighet till att ha vikter på noderna. Datatypen följer till stora delar definitionen av datatypen Graf på sidan 339 i kursboken (med undantag för 3 funktioner för att hantera vikter på kanterna) Lars-Erik Janlert,
 Torbjörn Wiberg Datatyper och algoritmer 2., [rev.] uppl.,Lund,
 Studentlitteratur, 2000, x, 387 s. ISBN 91-44-01364-7
 */

#ifndef GRAPH_H
#define GRAPH_H

#include <stdbool.h>
#include <stdlib.h>
#include "dlist.h"

//datatyp för en nod (valfri pekartyp)
typedef data vertice;

//Datatyp för en kant (består av två noder)
typedef struct edge {
    vertice v1; //startnod
    vertice v2; //slutnod
} edge;

/*Datatyp för jämförelsefunktion för noder. Ska returnera true om de
 två kanterna som anges som parameter är lika
 */
typedef bool equalityFunc(vertice v1, vertice v2);

/* Datatyp för funktion som avallokerar minne för en nod */
#ifndef __VERTICEFREEFUNC
#define __VERTICEFREEFUNC
typedef void verticeFreeFunc(vertice v);
#endif


/*Datatyp för funktion som avallokerar minne för ett kant värde */
#ifndef __EDGEDATAFREEFUNC
#define __EDGEDATAFREEFUNC
typedef void edgeDataFreeFunc(data d);
#endif


//Datatyp för grafen. Bör endast förändras via funktionerna i gränsytan.
typedef struct graph {
    dlist *nodes;
    equalityFunc *verticeEq;
    verticeFreeFunc *verticeFree;
    edgeDataFreeFunc *edgeDataFree;
} graph;

/*
 Syfte: Skapa en ny graf
 Parametrar: eq - Funktion för att avgöra om två noder (värden av typen
 vertice) i grafen är lika.
 Returvärde: Den nyskapade grafen
 Kommentarer: Om minneshanterare för nod och kant värden installerats mha
 setXXXMemHandler funktionerna så kommer minnet för dessa att
 avallokeras då värdena inte längre finns kvar i grafen.
 Annars ansvarar användaren av datatypen för att dessa
 värden avallokeras.
 */
graph *graph_empty(equalityFunc *eq);

/*
 Syfte: Installera en funktion som avallokerar minnet för etikett i en nod i grafen
 Parametrar: g - Grafen
 f - Funktion som avallokerar minnet för en nod.
 Kommentarer: Om funktionen inte anropats så krävs att användaren avallokerar
 minnet för värdet på noden.
 */
void graph_setVerticeMemHandler(graph *g,verticeFreeFunc *f);

/*
 Syfte: Installera en funktion som avallokerar minnet för etikett på en kant i grafen
 Parametrar: g - Grafen
 f - En funktion som avallokerar minnet för ett värde som är
 satt för en kant.
 Kommentarer: Om funktionen inte anropats så krävs att användaren avallokerar
 minnet för värdet på kanten.
 */
void graph_setEdgeDataMemHandler(graph *g,edgeDataFreeFunc *f);

/*
 Syfte: Stoppa in en nod i grafen
 Parametrar: g - grafen
 v - noden som ska sättas in
 Kommentarer: noden ska inte finnas i grafen sedan tidigare
 */
void graph_insertNode(graph *g,vertice v);

/*
 Syfte: Stoppa in en kant i grafen
 Parametrar: g - grafen
 e - kanten som ska sättas in.
 Kommentarer: Båda de ingående noderna i kanten måste finnas i grafen
 innan denna funktion anropas.
 */
void graph_insertEdge(graph *g,edge e);

/*
 Syfte: Kontrollera om grafen är tom
 Parametrar: g - grafen
 Returvärde: returnerar true om grafen är tom annars false
 Kommentarer:
 */
bool graph_isEmpty(graph *g);

/*
 Syfte: Kontrollera om grafen saknar kanter
 Parametrar: g - grafen
 Returvärde: true om grafen saknar kanter, annars false
 Kommentarer:
 */
bool graph_hasNoEdges(graph *g);

/*
 Syfte: Hämta ut en slumpmässig nod ur grafen
 Parametrar: g - grafen
 Returvärde: en nod som finns i grafen.
 Kommentarer:
 */
vertice graph_chooseNode(graph *g);

/*
 Syfte: Hämtar en lista med grannarna till en nod
 Parametrar: g - grafen
 v - Noden vars grannar ska hämtas
 Returvärde: listan med grannarna (innehåller värden av typen vertice)
 Kommentarer: Användaren ansvarar för att avallokera minnet för listan
 då denna inte behövs igen. Noderna i listan ägs av grafen.
 */
dlist *graph_neighbours(graph *g,vertice v);

/*
 Syfte: Tabort en nod ur trädet
 Parametrar: g - grafen
 v - noden som ska tas bort
 Kommentarer: Minnet för nodens etikett avallokeras bara om en funktion
 för detta satts via setVerticeMemHandler-funktionen.
 */
void graph_deleteNode(graph *g,vertice v);

/*
 Syfte: Tar bort en kant från grafen
 Parametrar: g - grafen
 e - kanten som ska tas bort
 Kommentarer: Minne för värden på etiketten avallokeras endast om en funktion
 för att göra detta har satta via setEdgeDataMemHandler-funktionen
 */
void graph_deleteEdge(graph *g,edge e);

/*
 Syfte: Undersöka om en kant har ett associerat värde satt i grafen
 Parametrar: g - grafen
 Returvärde: returnerar true om kanten har ett värde satt annars false
 Kommentarer:
 */
bool graph_hasEdgeLabel(graph *g,edge e);

/*
 Syfte: Associera ett värde med en kant i grafen
 Parametrar: g - Grafen.
 e - Kanten som ska få ett värde.
 label - Värdet som kanten ska ha.
 Kommentarer: kanten ska finnas i grafen innan anropet av denna funktion.
 */
void graph_setEdgeLabel(graph *g,edge e,data label);

/*
 Syfte: Hämta ett värde på en kant
 Parametrar: g - grafen
 e - kanten vars värde ska hämtas.
 Returvärde: värdet på kanten
 Kommentarer: Ej definierad om en kant saknas eller om ett värde ej satts för
 kanten
 */
data graph_inspectEdgeLabel(graph *g,edge e);

/*
 Syfte: Avallokera allt minne som upptas av grafen
 Parametrar: g - grafen
 Kommentarer: Om minneshanterare installerats för att ta hand om kanter
 och nodetiketter kommer även minnet för dessa att avallokeras.
 */
void graph_free(graph *g);

#endif