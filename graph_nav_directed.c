/*
 * Copyright 2012 Johan Eliasson (johane@cs.umu.se). Tillåtelse ges för användning på kurserna
 * i Datastrukturer och algoritmer vid Umeå Universitet. All annan användning kräver författarens
 * tillstånd.
 */

#include "graph_nav_directed.h"

//Datatyp som används internt i implementationen. Ingår ej i gränsytan
//och bör ej utnyttjas av användaren
typedef struct node {
    vertice v;
    dlist *neigbours;
}node;

//Datatyp som används internt i implementationen. Ingår ej i gränsytan
//och bör ej utnyttjas av användaren
typedef struct {
    vertice v;
    data value;
} edge_internal;

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
graph *graph_empty(equalityFunc *eq) {
    graph *theGraph=calloc(1,sizeof(graph));
    theGraph->nodes=dlist_empty();
    theGraph->verticeEq=eq;
    return theGraph;
}

/*
 Syfte: Installera en funktion som avallokerar minnet för etikett i en nod i grafen
 Parametrar: g - Grafen
 f - Funktion som avallokerar minnet för en nod.
 Kommentarer: Om funktionen inte anropats så krävs att användaren avallokerar
 minnet för alla etiketter manuellt.
 */
void graph_setVerticeMemHandler(graph *g,verticeFreeFunc *f) {
    g->verticeFree=f;
}

/*
 Syfte: Installera en funktion som avallokerar minnet för etikett på en kant i grafen
 Parametrar: g - Grafen
 f - En funktion som avallokerar minnet för ett värde som är
 satt för en kant.
 Kommentarer: Om funktionen inte anropats så krävs att användaren avallokerar
 minnet för värdet på kanten.
 */
void graph_setEdgeDataMemHandler(graph *g,edgeDataFreeFunc *f) {
    g->edgeDataFree=f;
}

//Följande funktion är bara en hjälpfunktion för minneshantering. Ej med i gränsytan
void freeInternalEdge(graph *g,edge_internal *e_int) {
    if(g->edgeDataFree && e_int->value) {
        g->edgeDataFree(e_int->value);
    }
    free(e_int);
}

/*
 Syfte: Stoppa in en nod i grafen
 Parametrar: g - grafen
 v - noden som ska sättas in
 Kommentarer: noden ska inte finnas i grafen sedan tidigare
 */
void graph_insertNode(graph *g,vertice v) {
    node *newNode=malloc(sizeof(node));
    newNode->v=v;
    newNode->neigbours=dlist_empty();
    dlist_insert(g->nodes,dlist_first(g->nodes),newNode);
}

/*
 Syfte: Stoppa in en kant i grafen
 Parametrar: g - grafen
 e - kanten som ska sättas in.
 Kommentarer: Båda de ingående noderna i kanten måste finnas i grafen
 innan denna funktion anropas.
 */
void graph_insertEdge(graph *g,edge e) {
    edge_internal *e_int1=calloc(1,sizeof(edge_internal));
    vertice vertice2=NULL;
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);
        p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v=n->v;
        if(g->verticeEq(e.v1,v)) {
            dlist_insert(n->neigbours,dlist_first(n->neigbours),e_int1);
        }
        else if (g->verticeEq(e.v2,v)) {
            vertice2=v;
        }
    }
    e_int1->v=vertice2;
}

/*
 Syfte: Kontrollera om grafen är tom
 Parametrar: g - grafen
 Returvärde: returnerar true om grafen är tom annars false
 Kommentarer:
 */
bool graph_isEmpty(graph *g) {
    return dlist_isEmpty(g->nodes);
}

/*
 Syfte: Kontrollera om grafen saknar kanter
 Parametrar: g - grafen
 Returvärde: true om grafen saknar kanter, annars false
 Kommentarer:
 */
bool graph_hasNoEdges(graph *g) {
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);
        p=dlist_next(g->nodes,p)) {
        node *node=dlist_inspect(g->nodes,p);
        if(!dlist_isEmpty(node->neigbours))
            return false;
    }
    return true;
}

/*
 Syfte: Hämta ut en slumpmässig nod ur grafen
 Parametrar: g - grafen
 Returvärde: en nod som finns i grafen.
 Kommentarer:
 */
vertice graph_chooseNode(graph *g) {
    node *n=dlist_inspect(g->nodes,dlist_first(g->nodes));
    return n->v;
}

/*
 Syfte: Hämtar en lista med grannarna till en nod
 Parametrar: g - grafen
 v - Noden vars grannar ska hämtas
 Returvärde: listan med grannarna (innehåller värden av typen vertice)
 Kommentarer: Användaren ansvarar för att avallokera minnet för listan
 då denna inte behövs igen. Noderna i listan ägs av grafen.
 */
dlist *graph_neighbours(graph *g,vertice v) {
    dlist *list=dlist_empty();
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);
        p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        if(g->verticeEq(v,n->v)) {
            for(dlist_position p2=dlist_first(n->neigbours);
                !dlist_isEnd(n->neigbours,p2);
                p2=dlist_next(n->neigbours,p2)) {
                edge_internal *myedge=dlist_inspect(n->neigbours,p2);
                dlist_insert(list,dlist_first(list),myedge->v);
            }
        }
    }
    return list;
}

/*
 Syfte: Tabort en nod ur trädet
 Parametrar: g - grafen
 v - noden som ska tas bort
 Kommentarer: Minnet för nodens etikett avallokeras bara om en funktion
 för detta satts via setVerticeMemHandler-funktionen.
 */
void graph_deleteNode(graph *g,vertice v) {
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v2=n->v;
        if(g->verticeEq(v,v2)) {
            dlist_remove(g->nodes,p);
            while(!dlist_isEmpty(n->neigbours)) {
                edge_internal *e_int=dlist_inspect(n->neigbours,dlist_first(n->neigbours));
                freeInternalEdge(g,e_int);
                dlist_remove(n->neigbours,dlist_first(n->neigbours));
            }
            dlist_free(n->neigbours);
            if(g->verticeFree && n->v)
                g->verticeFree(n->v);
            free(n);
            return;
        }
    }
}

/*
 Syfte: Hämta ett värde på en kant
 Parametrar: g - grafen
 e - kanten vars värde ska hämtas.
 Returvärde: värdet på kanten
 Kommentarer: Ej definierad om en kant saknas eller om ett värde ej satts för
 kanten
 */
data graph_inspectEdgeLabel(graph *g,edge e) {
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v=n->v;
        if(g->verticeEq(e.v1,v)) {
            for(dlist_position p2=dlist_first(n->neigbours);
                !dlist_isEnd(n->neigbours,p2);
                p2=dlist_next(n->neigbours,p2)) {
                edge_internal *e_int=dlist_inspect(n->neigbours,p2);
                if(g->verticeEq(e.v2,e_int->v)) {
                    return e_int->value;
                }
            }
        }
    }
    return NULL;
}

/*
 Syfte: Associera ett värde med en kant i grafen
 Parametrar: g - Grafen.
 e - Kanten som ska få ett värde.
 label - Värdet som kanten ska ha.
 Kommentarer: kanten ska finnas i grafen innan anropet av denna funktion.
 */
void graph_setEdgeLabel(graph *g,edge e,data label) {
    for(dlist_position p=dlist_first(g->nodes);
        !dlist_isEnd(g->nodes,p);p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v=n->v;
        if(g->verticeEq(e.v1,v)) { //If its the first vertice
            for(dlist_position p2=dlist_first(n->neigbours);
                !dlist_isEnd(n->neigbours,p2);
                p2=dlist_next(n->neigbours,p2)) {
                edge_internal *e_int=dlist_inspect(n->neigbours,p2);
                if(g->verticeEq(e.v2,e_int->v)) {
                    e_int->value=label;
                }
            }
        }
    }
}

/*
 Syfte: Undersöka om en kant har ett associerat värde satt i grafen
 Parametrar: g - grafen
 Returvärde: returnerar true om kanten har ett värde satt annars false
 Kommentarer:
 */
bool graph_hasEdgeLabel(graph *g,edge e) {
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v2=n->v;
        if(g->verticeEq(e.v1,v2)) {
            for(dlist_position p2=dlist_first(n->neigbours);
                !dlist_isEnd(n->neigbours,p2);
                p2=dlist_next(n->neigbours,p2)) {
                edge_internal *e_int=dlist_inspect(n->neigbours,p2);
                if(g->verticeEq(e.v2,e_int->v)) {
                    return e_int->value!=NULL;
                }
            }
        }
    }
    return false;
}

/*
 Syfte: Tar bort en kant från grafen
 Parametrar: g - grafen
 e - kanten som ska tas bort
 Kommentarer: Minne för värden på etiketten avallokeras endast om en funktion
 för att göra detta har satta via setEdgeDataMemHandler-funktionen
 */
void graph_deleteEdge(graph *g,edge e) {
    for(dlist_position p=dlist_first(g->nodes);!dlist_isEnd(g->nodes,p);
        p=dlist_next(g->nodes,p)) {
        node *n=dlist_inspect(g->nodes,p);
        vertice v2=n->v;
        if(g->verticeEq(e.v1,v2)) {
            for(dlist_position p2=dlist_first(n->neigbours);
                !dlist_isEnd(n->neigbours,p2);
                p2=dlist_next(n->neigbours,p2)) {
                edge_internal *e_int=dlist_inspect(n->neigbours,p2);
                if(g->verticeEq(e.v2,e_int->v)) {
                    freeInternalEdge(g,e_int);
                    dlist_remove(n->neigbours,p2);
                    break; //Only one edge possible between two nodes
                }
            }
        }
    }
}

/*
 Syfte: Avallokera allt minne som upptas av grafen
 Parametrar: g - grafen
 Kommentarer: Om minneshanterare installerats för att ta hand om kanter
 och nodetiketter kommer även minnet för dessa att avallokeras.
 */
void graph_free(graph *g) {
    while(!dlist_isEmpty(g->nodes)) {
        node *n=dlist_inspect(g->nodes,dlist_first(g->nodes));
        while(!dlist_isEmpty(n->neigbours)) {
            edge_internal *e_int=dlist_inspect(n->neigbours,dlist_first(n->neigbours));
            freeInternalEdge(g,e_int);
            dlist_remove(n->neigbours,dlist_first(n->neigbours));
        }
        graph_deleteNode(g,n->v);
    }
    dlist_free(g->nodes);
    free(g);
}