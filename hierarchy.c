#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hierarchy.h"


Tree found(Tree tree, char *manager_name)
{
    if(tree == NULL)
        return NULL;

    if(strcmp(tree->name, manager_name) == 0)
    {
        return tree;
    }

    for(int i = 0; i < tree->direct_employees_no; i++)
    {       
        Tree search = found(tree->team[i], manager_name);
        if(search != NULL)
            return search;
    }

    return NULL;
}

Tree insertAlpha(Tree current, Tree employee)
{
    int gasit = 0;

    // pt fiecare nod pe care il inseram, modificam contorul direct_employees_no si dimensiunea vectorului alocat dinamic team
    current->direct_employees_no++;
    current->team = realloc(current->team, 2 * current->direct_employees_no * sizeof(Tree));

    //daca nu avea elem vectorul inainte de inserarea actuala
    if(current->direct_employees_no == 1)
    {
        // realizam cele 2 legaturi
        current->team[0] = employee;
        employee->manager = current;
    }
    else
    {
        for(int i = current->direct_employees_no - 2; i >= 0; i--)
        {
            // cautam loc de inserat astfel incat sa se respecte ordinea lexicografica
            if(strcmp(employee->name, current->team[i]->name) > 0 ||( strlen(employee->name) > strlen(current->team[i]->name) && strstr(employee->name, current->team[i]->name) != NULL))
            {
                //realizam legaturile
                current->team[i + 1] = employee;
                employee->manager = current;
                gasit = 1;
                break;
            }

            //daca nu am gasit inaintam in vector mutand la dreapta el cu el
            current->team[i + 1] = current->team[i];
        }
                
        //daca nu a gasit loc inseamna ca trebuie inserat la inceput
        if(gasit == 0)
        {
            current->team[0] = employee;
            employee->manager = current;
        }
    }
    return current;
}

Tree deleteAlpha(Tree current, Tree employee)
{
    int gasit = 0;

    for(int i = 0; i < current->direct_employees_no; i++)
    {
        // cautam dupa nume angajatul de concediat printre membrii echipei
        if(strcmp(current->team[i]->name, employee->name) == 0)
        {
            gasit = 1;
            current->team[i] = NULL;
        }
        if(gasit == 1)
        {   
            // daca am eliberat un loc mutam vectorul de la poz respectiva
            // la stanga cu o casuta
            current->team[i] = current->team[i + 1];
        }
    }
    current->direct_employees_no--;
    return current;
}

Tree deleteTeam(Tree tree, Tree manager)
{
    int i;
    if(tree == NULL)
        return NULL;
    
    for( i = 0; i < tree->direct_employees_no; i++)
    {
        // stergerea se realizeaza apeland functia fire de concediere 
        Tree deSters = fire(tree, tree->team[i]->name);
        return deleteTeam(deSters, manager);
    }

    if(i == tree->direct_employees_no)
    {   
        // stergem leg dintre manager si nodul radacina de sters
        if(tree != NULL)
            manager = fire(manager, tree->name);
        return NULL;
    }   
}

void listare(Tree tree, Tree list)
{
     int i;

    if(tree == NULL)
    {    
        return;
    }
  
    for( i = 0; i < tree->direct_employees_no; i++)
    {   
        // adaugam nodurile rand pe rand in lista pentru a realiza ordonarea alfabetica
        list = hire(list, tree->team[i]->name, list->name);    
        listare( tree->team[i], list);
    }   
     
    return;
}

void parcurgereNivel(Tree tree, Tree list, FILE* f, int level, int currentLevel)
{
    int i;
    if(tree == NULL)
        return;

    if(currentLevel == level )
    {  
        // se adauga elementele de pe acelasi nivel pe nivelul 1 al unui nou arbore
        // pt a mentine ordinea alfabetica
        list = hire(list, tree->name, list->name);
        return;
    }

    for( i = 0; i < tree->direct_employees_no; i++)
    {
         parcurgereNivel( tree->team[i], list, f, level, currentLevel + 1);
    }   
    
    return;
}

void cautareMax(Tree tree, int* max)
{
    int i;
    if(tree == NULL)
        return;

    if(tree->direct_employees_no > *max)
    {  
        (*max) = tree->direct_employees_no;
    }

    for( i = 0; i < tree->direct_employees_no; i++)
    {
        cautareMax(tree->team[i], max);

    }   
    
    return;
}

void cautareBestManagers(Tree tree, Tree list, int max)
{
    int i;
    if(tree == NULL)
        return;

    if(tree->direct_employees_no == max)
    {  
        list = hire(list, tree->name, list->name);
    }

    for( i = 0; i < tree->direct_employees_no; i++)
    {
        cautareBestManagers(tree->team[i], list, max);

    }   
    
    return;
}

void parcurgereStd(Tree tree)
{
 int i;

    if(tree == NULL)
    {    
        return;
    }
    else
    {
        if(tree->manager == NULL)
            printf( "%s ", tree->name);
        else
            printf("%s-%s ", tree->name, tree->manager->name);
                
        for( i = 0; i < tree->direct_employees_no; i++)
            parcurgereStd( tree->team[i]);

        if(i == tree->direct_employees_no && tree->manager == NULL)
            printf("\n");      
    }   
}

/* Elibereaza memoria alocata nodurilor din arbore
 *
 * tree: ierarhia existenta
 */
void destroy_tree(Tree tree) {
    
    if(tree == NULL)
        return;
    
    for(int i = 0 ; i < tree->direct_employees_no; i++)
        destroy_tree(tree->team[i]);

    free(tree->name);
    free(tree->team);
    tree->team = NULL;
    free(tree);
    tree = NULL;
   
}

/* Adauga un angajat nou in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 * manager_name: numele sefului noului angajat
 *
 * return: intoarce ierarhia modificata. Daca tree si manager_name sunt NULL, 
           atunci employee_name e primul om din ierarhie.
 */
Tree hire(Tree tree, char *employee_name, char *manager_name) {
    
    // alocam memorie si intializam angajatul ce va urma sa fie inserat
    Tree employee = calloc(1, sizeof(TreeNode));
    employee->name = calloc(10, sizeof(char));
    strcpy(employee->name, employee_name);
    employee->manager = NULL;
    employee->team = NULL;
    employee->direct_employees_no = 0;
    
    if(tree == NULL)
    {
        //daca nu avem radacina, noul de inserat devine radacina
        tree = employee;
        return tree;
    }
    else
    {   //gasim nodul de care trebuie legat nodul de inserat    
        Tree current = found(tree, manager_name);

        //functia insert leaga nodul curent de nodul parinte corespunzator
        current = insertAlpha(current, employee);
        return tree;           
    } 
}

/* Sterge un angajat din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului concediat
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire(Tree tree, char *employee_name) {
    
    Tree copie = tree;
    Tree current = found(tree, employee_name);

    if(current == NULL || current->manager == NULL)
        return copie;
    else
    {
        Tree manager = current->manager;
        
        for(int i  = 0; i < current->direct_employees_no; i++)
        {
            // schimbam legaturile echipei nodului de concediat 
            manager = insertAlpha(manager, current->team[i]);
        }

        // stergem legatura nodului de concediat cu managerul lui
        manager = deleteAlpha(manager, current);
        // eliberam memorie
        free(current->name);
        free(current->team);
        free(current);

        return copie;
    }

}

/* Promoveaza un angajat in ierarhie. Verifica faptul ca angajatul e cel putin 
 * pe nivelul 2 pentru a putea efectua operatia.
 * 
 * tree: ierarhia existenta
 * employee_name: numele noului angajat
 *
 * return: intoarce ierarhia modificata.
 */
Tree promote(Tree tree, char *employee_name) {
    
    Tree promovat = found(tree, employee_name);

    if(promovat == NULL || promovat->manager == NULL || promovat->manager->manager == NULL )
        return tree;
    else
    {
        Tree newManager = promovat->manager->manager;
        Tree manager = promovat->manager;

        for(int i = 0; i < promovat->direct_employees_no; i++)
        {
            //schimbam leg echipei angajatului de promovat astfel incat sa apartina de managerul lui actual
            manager = insertAlpha(manager, promovat->team[i]);
        }

        int i = 0;
        int nr_angajati_initial = promovat->direct_employees_no;

        while(i < nr_angajati_initial)
        {
            //stergem legaturile angajatului promovat la echipa sa
            promovat = deleteAlpha(promovat, promovat->team[0]);
            i++;
        }
        // inseram angajatul promovat printre copii managerului managerului
        newManager = insertAlpha(newManager, promovat);

        //stergem legatura dintre fostul manager si angajatul promovat
        manager = deleteAlpha(manager, promovat);
        
        return tree;
        
    }
}

/* Muta un angajat in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_employee(Tree tree, char *employee_name, char *new_manager_name) {
    
    Tree angajatMutat = found(tree, employee_name);
    Tree newManager = found(tree, new_manager_name);

    if(angajatMutat == NULL || angajatMutat->manager == NULL || angajatMutat->manager == newManager)
        return tree;
    else
    {
        Tree manager = angajatMutat->manager;

        for(int i = 0; i < angajatMutat->direct_employees_no; i++)
        {
            manager = insertAlpha(manager, angajatMutat->team[i]);
        }

        int i = 0;
        int nr_angajati_initial = angajatMutat->direct_employees_no;

        while(i < nr_angajati_initial)
        {
            angajatMutat = deleteAlpha(angajatMutat, angajatMutat->team[0]);
            i++;
        }

        newManager = insertAlpha(newManager, angajatMutat);
        manager = deleteAlpha(manager, angajatMutat);

        return tree;
    }
}

/* Muta o echipa in ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei mutate
 * new_manager_name: numele noului sef al angajatului
 *
 * return: intoarce ierarhia modificata.
 */
Tree move_team(Tree tree, char *employee_name, char *new_manager_name) {
    
    Tree echipaMutat = found(tree, employee_name);
    Tree newManager = found(tree, new_manager_name);

    if(echipaMutat == NULL || echipaMutat->manager == NULL || echipaMutat->manager == newManager)
        return tree;
    else
    {
        Tree manager = echipaMutat->manager;
        newManager = insertAlpha(newManager, echipaMutat);
        //stergem vechea legatura dintre vechiul manager si anagajatul mutat
        manager = deleteAlpha(manager, echipaMutat);

        return tree;
    }
}

/* Concediaza o echipa din ierarhie.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei concediate
 *
 * return: intoarce ierarhia modificata.
 */
Tree fire_team(Tree tree, char *employee_name) {
    
    Tree echipa = found(tree, employee_name);
    echipa = deleteTeam(echipa, echipa);

    return tree;

}

/* Afiseaza toti angajatii sub conducerea unui angajat.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * employee_name: numele angajatului din varful echipei
 */
void get_employees_by_manager(FILE *f, Tree tree, char *employee_name) {

    Tree manager = found(tree, employee_name);

    if(manager == NULL)
    {
        fprintf(f, "\n");
        return;
    }

    Tree list = hire(NULL, manager->name, NULL);
    listare( manager, list);

    //adaugam si managerul in lista care era si radacina pentru a asigura ordinea alfabetica
    list = hire(list, list->name, list->name);

    for(int i = 0; i < list->direct_employees_no; i++)
        fprintf(f, "%s ", list->team[i]->name);

    fprintf(f, "\n");

    destroy_tree(list);
     
}

/* Afiseaza toti angajatii de pe un nivel din ierarhie.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 * level: nivelul din ierarhie
 */
void get_employees_by_level(FILE *f, Tree tree, int level) {
    
    if(level < 0)
    {
        fprintf(f, "\n");
        return;
    }    

    if(level == 0)
    {
        fprintf(f, "%s \n", tree->name);
        return;
    }    
    
    Tree list = hire(NULL, tree->name, NULL);
    parcurgereNivel(tree, list, f, level, 0);
    // la iesirea din functia parcurgereNivel toate el de pe nivelul dat vor aparea
    // alfabetic pe nivelul 1 

    // afisare
    for(int i = 0; i < list->direct_employees_no; i++)
        fprintf(f, "%s ", list->team[i]->name);

    fprintf(f, "\n");
    destroy_tree(list);

}

/* Afiseaza angajatul cu cei mai multi oameni din echipa.
 * 
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void get_best_manager(FILE *f, Tree tree) {
    
    int max = 0;
    // cautam nr maxim de angajati din care e alcatuita o echipa si il retinem in max
    cautareMax(tree, &max);
    // retinem in list prin apelarea functiei toti managerii care au nr maxim de angajati
    Tree list = hire(NULL, tree->name, NULL);
    cautareBestManagers(tree, list, max);

    for(int i = 0; i < list->direct_employees_no; i++)
        fprintf(f, "%s ", list->team[i]->name);

    fprintf(f, "\n");
    destroy_tree(list);

}

/* Reorganizarea ierarhiei cu un alt angajat in varful ierarhiei.
 * 
 * tree: ierarhia existenta
 * employee_name: numele angajatului care trece in varful ierarhiei
 */
Tree reorganize(Tree tree, char *employee_name) {
    
    Tree varf = found(tree, employee_name);
   
    if(varf == NULL || varf->manager == NULL)
    {
        return tree;
    }   
    else
    {
        Tree nod = varf;
        Tree varfCopie = varf;
        char** matrixManageri = malloc(5 * sizeof(char*));

        // alocam un vector de stringuri in care vom retine nume de manageri
        for(int i = 0; i < 5; i++)
        {
            matrixManageri[i] = calloc(4, sizeof(char));
        }

        int panLaVarf = 0; // vom retine aici nivelul pe care se afla angajatul dat

        while(nod->manager != NULL)
        {
            // parcurgem ierarhia de la angajat in sus
            nod = nod->manager;
            // retinem in matrice numele managerilor aflati deasupra angajatului in ierarhie
            if(nod != NULL)
                strcpy(matrixManageri[panLaVarf], nod->name);  
            panLaVarf++;
        }   

        int index = 0;
        Tree Manager;

        while(index < panLaVarf)
        {
            Manager = found(tree, matrixManageri[index]);
            char* copie = Manager->name;

            // trecem managerul printre copiii varfului
            varf = insertAlpha(varf, Manager);
            // stergem varful dintre copiii managerului anterior
            Manager = deleteAlpha(Manager, varf);
            // varful devine managerul anterior gasit
            varf = found(varfCopie, copie);

            index++;           
        }

        varfCopie->manager = NULL;

        for(int i = 0; i < 5; i++)
        {
            free(matrixManageri[i]);
        }
        free(matrixManageri);
        return varfCopie;
    }
}

/* Parcurge ierarhia conform parcurgerii preordine.
 *
 * f: fisierul in care se va face afisarea
 * tree: ierarhia existenta
 */
void preorder_traversal(FILE *f, Tree tree) {
    
    int i;
    if(tree == NULL)
    {    
        return;
    }
    else
    {
        if(tree->manager == NULL)
            fprintf(f, "%s ", tree->name);
        else
            fprintf(f, "%s-%s ", tree->name, tree->manager->name);
                
        for( i = 0; i < tree->direct_employees_no; i++)
            preorder_traversal(f, tree->team[i]);

        if(i == tree->direct_employees_no && tree->manager == NULL)
            fprintf(f, "\n");      
    }
}

