//
//  isConnected.c
//  OU5
//
//  Created by Fredrik Östlund on 2016-03-02.
//  Copyright © 2016 Fredrik Östlund. All rights reserved.
//  id15fod
//  fras0033

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "hashtable.h"
#include "graph_nav_directed.h"
#include "queue.h"
/*
 * Purpose: Makes sure the keys not end up on the same place
 * Parameters:
 * Nodekey - the actual key
 * Comment:
 */
unsigned hashFunc(ht_key_t NodeKey){
    
    char *strKey = NodeKey;
    unsigned hashvalue = 0;
    int index = 0;
    
    while(strKey[index]){
        
        hashvalue ^= strKey[index]<<(index%sizeof(int));
        index++;
    }
    return hashvalue;
}
/*
 * Purpose: Compare two given keys
 * Parameters:
 * key1 - key to compare
 * key2 - key to compare
 * Comment: Returns true if the keys are equal and false if they are not
 */
bool keyCompare(ht_key_t key1, ht_key_t key2) {
    if (strcmp(key1, key2) == 0) {
        return true;
    }
    else {
        return false;
    }
}
/*
 * Purpose: Compare two given vertices
 * Parameters: 
 * v1 - vertice to compare
 * v2 - vertice to compare
 * Comment: Returns true if the vertices are equal and false if they are not
 */
bool verticeCompare(vertice v1, vertice v2) {
    if (strcmp((char*)v1, (char*)v2) == 0) {
        return true;
    }
    else {
        return false;
    }
}
/*
 * Purpose: Check if a node in the graph is visited
 * Parameters:
 * table - hashtable where the nodes are stored
 * node - node to check if visited
 * Comment: If node is not visited it is put in the hashtable and then is 
 * visited
 */
bool checkVisited(hashtable_t *table, char* node) {
    if (hashtable_lookup(table, node) == NULL) {
        
        char* insertNode = calloc(41, sizeof(char));
        strcpy(insertNode, node);
        
        char* insertKey = calloc(41, sizeof(char));
        strcpy(insertKey, node);
        
        hashtable_insert(table, insertKey, insertNode);
        return false;
    }
    else {
        return true;
    }
}
/*
 * Purpose: Traverse the graph
 * Parameters:
 * node1 - starting node
 * node2 - endnode
 * myGraph - the graph to be traversed
 * table - table that is responsible for if a node is visited or not
 * list - list where neighbours are stored
 * Comment: traversing the graph with a breadth first algorithm
 */
bool breadthFirst(char* node1, char* node2, graph *myGraph, hashtable_t *table,
                  dlist *list){
    queue *q;
    q = queue_empty();
    
    char *node;
    

    dlist_position p;
    
    checkVisited(table, node1);
    queue_enqueue(q, node1);
    
    while (!queue_isEmpty(q)) {
        node = queue_front(q);
        
        queue_dequeue(q);
        list = graph_neighbours(myGraph, node);
        
        p = dlist_first(list);
        
        while (!dlist_isEnd(list, p)) {
            
            if (!checkVisited(table, dlist_inspect(list, p))) {
                
                if (verticeCompare(dlist_inspect(list, p), node2)) {
                    dlist_free(list);
                    queue_free(q);
                    return true;
                }
                else {
                    queue_enqueue(q, dlist_inspect(list, p));
                }
            }
            p =dlist_next(list, p);
        }
        dlist_free(list);
    }
    queue_free(q);
    return false;
}

int main ( int argc, const char * argv[]) {
    
    FILE *myFile;
    int nrOfEdges;
    char* node1 = calloc(41, sizeof(char));
    char* node2 = calloc(41, sizeof(char));
    char* value1 = NULL;
    char* value2 = NULL;
    bool traverse;
    
    myFile = fopen(argv[1], "r");
    if (argv[1] == NULL) {
        fprintf(stderr, "ERROR: No file input\n");
        exit(EXIT_FAILURE);
    }
    fscanf(myFile, "%d", &nrOfEdges);
    
    hashtable_t *Table;
    Table = hashtable_empty(nrOfEdges *2, hashFunc, keyCompare);
    
    hashtable_t *visitedTable;
    
    graph *nodeGraph;
    nodeGraph = graph_empty(verticeCompare);
    edge e;
    graph_setEdgeDataMemHandler(nodeGraph, free);
    graph_setVerticeMemHandler(nodeGraph, free);
    
    dlist *list;
    list = dlist_empty();
    dlist_setMemHandler(list, free);
    /*
     * Putting in all unique nodes to the graph
     */
    while (!feof(myFile)) {
        
        fscanf(myFile, "%s", node1);
        fscanf(myFile, "%s", node2);
        
        if (hashtable_lookup(Table, node1) == NULL) {
            value1 = calloc(41, sizeof(char));
            strcpy(value1, node1);
            
            hashtable_insert(Table, value1, value1);
            graph_insertNode(nodeGraph, value1);
        }
        if(hashtable_lookup(Table, node2) == NULL) {
            value2 = calloc(41, sizeof(char));
            strcpy(value2, node2);
            
            hashtable_insert(Table, value2, value2);
            graph_insertNode(nodeGraph, value2);
        }
        e.v1 = node1;
        e.v2 = node2;
        graph_insertEdge(nodeGraph, e);
    }
    
    char *inputNode1 = calloc(41, sizeof(char));
    char *inputNode2 = calloc(41, sizeof(char));
    int running = 1;
    /* 
     * Reading input from user which nodes to check if there is a way between
     */
    do {
        visitedTable = hashtable_empty(nrOfEdges*2, hashFunc, keyCompare);
        hashtable_setKeyMemHandler(visitedTable, free);
        hashtable_setValueMemHandler(visitedTable, free);
        
        printf("Enter origin and destination (quit to exit): ");
        scanf("%s", inputNode1);
        
        if (strcmp(inputNode1, "quit") == 0) {
            hashtable_free(visitedTable);
            break;
        }
        scanf(" %s", inputNode2);
        traverse = breadthFirst(inputNode1, inputNode2, nodeGraph, visitedTable,
                                list);
        
        if (traverse) {
            printf("%s and %s are connected\n", inputNode1, inputNode2);
        }
        else {
            printf("%s and %s are not connected\n", inputNode1, inputNode2);
        }
        hashtable_free(visitedTable);
    }while (running);
    /*
     * Freeing the allocated memory
     */
    
    fclose(myFile);
    free(node1);
    free(node2);
    free(inputNode1);
    free(inputNode2);
    dlist_free(list);
    graph_free(nodeGraph);
    hashtable_free(Table);
    return 0;
}


