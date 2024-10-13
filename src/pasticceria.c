#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_LENGTH_NOME 20

enum Colore
{
    RED,
    BLACK
};

//* STRUCT-------------------------------------------------------------------------------------------------------------------------------------

struct Ordine;
struct CodaOrdiniPronti;
struct IngredienteRicetta;
struct Ricetta;
struct IngredienteMagazzino;
struct LottoIngrediente;
struct Magazzino;

typedef struct Ordine
{
    int tempoOrdine;
    struct Ricetta *ricetta;
    unsigned short quantita;
    unsigned short pesoOrdine;
    struct Ordine *next;
} Ordine;

typedef struct CodaOrdiniPronti
{
    struct Ordine *testa;
    struct Ordine *coda;
} CodaOrdiniPronti;

typedef struct IngredienteRicetta
{
    unsigned short quantita;
    struct IngredienteMagazzino *ingredienteMagazzino;
    struct IngredienteRicetta *next;
} IngredienteRicetta;

typedef struct Ricetta
{
    char nome_ricetta[MAX_LENGTH_NOME];
    unsigned short peso_ricetta;
    struct IngredienteRicetta *ingredientiRicetta;
    struct Ricetta *left;
    struct Ricetta *right;
    struct Ricetta *p;

} Ricetta;

typedef struct IngredienteMagazzino
{
    char nome_ingrediente[MAX_LENGTH_NOME];
    unsigned short quantitaTotale;
    struct LottoIngrediente *listaLotti;

    enum Colore colore;
    struct IngredienteMagazzino *left;
    struct IngredienteMagazzino *right;
    struct IngredienteMagazzino *p;

} IngredienteMagazzino;

typedef struct Magazzino
{
    struct IngredienteMagazzino *root;
    struct IngredienteMagazzino *NIL;
} Magazzino;

typedef struct LottoIngrediente
{
    int scadenza;
    unsigned short quantita;
    struct LottoIngrediente *next;
} LottoIngrediente;

typedef struct
{
    struct Ordine *testa;
    struct Ordine *coda;
} ListaAttesa; // utile per inserimento in coda

//* Globali------------------------------------------------------------------------------------------------------------------------------------

int Er;
int tempo;
unsigned short periodicita;
int capienza;
CodaOrdiniPronti codaOrdiniPronti;
Ricetta *ricettario;
Magazzino magazzino;
ListaAttesa listaAttesa;
FILE *file = NULL;

//* FUNZIONI-----------------------------------------------------------------------------------------------------------------------------------
void configura();

bool aggiungiRicetta();
void rimuoviRicetta();
bool processaOrdine();
void processaRifornimento();

// Gestione Magazzino [RBTree]
void inizializzaMagazzino();                                                                                                                                 // inzializzazione RBTree Magazzino
IngredienteMagazzino *minMagazzino(IngredienteMagazzino *root);                                                                                              // min RBTree Magazzino
IngredienteMagazzino *inserisciInMagazzino(IngredienteMagazzino *root, char *nome_ingrediente, unsigned short quantitaTotale, LottoIngrediente *listaLotti); // Insert in RBTree Corman
void insertFixupMagazzino(IngredienteMagazzino *z);                                                                                                          // Insert Fix Up RBTree Corman
IngredienteMagazzino *trovaInMagazzino(IngredienteMagazzino *x, char *nome_ingrediente);                                                                     // ricerca e restituzione Ingrediente in magazzino
void modificaScorte(IngredienteMagazzino *ingrediente, unsigned short quantita_prodotto, int scadenza);                                                      // aggiunta o rimozione di quantita_prodotto dall'ingredionete in magazzino e aggiornamento listaLotti
bool ingredientiSufficienti(Ricetta *ricetta, unsigned short quantita);                                                                                      // controllo qunatita ingrediente in magazzino sufficiente per ogni ingrediente di nome_ricetta
void controlloScaduti(IngredienteMagazzino *ingrediente);                                                                                                    // controllo scadenze lotti degli ingredienti in magazzino

// Gestione Lotti in Magazzino [Lista con Inserimento Ordinato e Rimozione solo dalla testa]
void aggiungiLotto(LottoIngrediente **testa, int scadenza, unsigned short quantita);
void rimuoviPrimoLotto(LottoIngrediente **testa);
void rimuoviListaLotti(LottoIngrediente *listaLotti);
void rimuoviIngredientiUtilizzati(Ricetta *ricetta, unsigned short quantita);
LottoIngrediente *duplicaListaLotti(LottoIngrediente *listaLotti);

// Gestione Ricettario [BST]
Ricetta *prendiInRicettario(Ricetta *x, char *nome_ricetta);                                                         // ricerca ricorsiva su BST Cormen
Ricetta *minRicettario(Ricetta *root);                                                                               // ricerca min su BST Cormen
void transplantRicettario(Ricetta *u, Ricetta *v);                                                                   // trasplant BST Cormen per la remove
void inserisciInRicettario(char *nome_ricetta, unsigned short peso_ricetta, IngredienteRicetta *ingredientiRicetta); // inserimento BST Cormen
void rimuoviDaRicettario(char *nome_ricetta);                                                                        // rimozione BST Cormen

// Gestione Ingredienti in Ricetta [Lista non ordinata senza rimozione]
void inserisciIngredienteRicetta(IngredienteRicetta **testa, char *nome_ingrediente, unsigned short quantita);
void cancellaListaIngredienti(IngredienteRicetta *listaIngredienti);

// Gestione Ordini in Attesa [Lista con inserimento in coda e rimozione su iterazione]
void inserisciInListaAttesa(int tempoOrdine, Ricetta *ricetta, unsigned short quantita);
void processaOrdiniInAttesa();
bool presenteInListaAttesa(char *nome_ricetta);

// Gestione Ordini Pronti [Lista con inserimento da coda, inserimento ordinato, rimozione solo da testa ]
Ordine *creaOrdine(int tempoOrdine, Ricetta *ricetta, unsigned short quantita, unsigned short pesoOrdine);
void inizializzaCodaOrdiniPronti();
bool isEmptyCodaOrdiniPronti();
void inserisciInCodaOrdiniPronti(int tempoOrdine, Ricetta *ricetta, unsigned short quantita);
void inserisciOrdinatoInCodaOrdiniPronti(int tempoOrdine, Ricetta *ricetta, unsigned short quantita);
Ordine *rimuoviDaCodaOrdiniPronti();
bool presenteInCodaOrdiniPronti(char *nome_ricetta);

// Gestione Corriere
void configuraCorriere();
void corriCorriCorriere();
void inserisciInListaCaricamento(Ordine **testa, Ordine *nuovoOrdine);

//* main --------------------------------------------------------------------------------------------------------------------------------------

int main()
{

    configura();

    char c;
    do
    {
        corriCorriCorriere();
        controlloScaduti((&magazzino)->root);

        c = getc_unlocked(file);
        switch (c)
        {

        case 'a': // aggiungi_ricetta
        {
            if (aggiungiRicetta())
            {
                printf("aggiunta\n");
            }
            else
            {
                printf("ignorato\n");
            }
        }
        break;

        case 'r':
        {
            getc_unlocked(file);

            switch (getc_unlocked(file))
            {
            case 'm': // rimuovi_ricetta
            {
                rimuoviRicetta();
            }
            break;
            case 'f': // rifornimento
            {
                processaRifornimento();
                printf("rifornito\n");
            }
            break;
            }
        }
        break;

        case 'o': // ordine
        {

            if (processaOrdine())
            {
                printf("accettato\n");
            }
            else
            {
                printf("rifiutato\n");
            }
        }
        break;

        default:
            break;
        }

        tempo++;

    } while (c != EOF);

    return 0;
}

// FUNZIONI-----------------------------------------------------------------------------------------------------------------------------------

void configura()
{
    file = stdin;

    tempo = 0;
    inizializzaMagazzino();
    ricettario = NULL;
    (&listaAttesa)->testa = NULL;
    (&listaAttesa)->coda = NULL;
    inizializzaCodaOrdiniPronti();
    configuraCorriere();
}

// Gestione Magazzino [RBTree]
void inizializzaMagazzino() // inzializzazione RBTree Magazzino
{
    (&magazzino)->NIL = (IngredienteMagazzino *)malloc(sizeof(IngredienteMagazzino));
    (&magazzino)->NIL->colore = BLACK;
    (&magazzino)->root = (&magazzino)->NIL;
}

IngredienteMagazzino *minMagazzino(IngredienteMagazzino *root) // min RBTree Magazzino
{
    while (root->left != (&magazzino)->NIL)
        root = root->left;
    return root;
}

void leftRotateMagazzino(IngredienteMagazzino *x) // left rotate RBTree Magazzino
{
    IngredienteMagazzino *y = x->right;
    x->right = y->left;

    if (y->left != (&magazzino)->NIL)
        y->left->p = x;

    y->p = x->p;

    if (x->p == (&magazzino)->NIL)
        (&magazzino)->root = y;
    else if (x == x->p->left)
        x->p->left = y;
    else
        x->p->right = y;

    y->left = x;
    x->p = y;
}

void rightRotateMagazzino(IngredienteMagazzino *x) // right rotate RBTree Magazzino
{
    IngredienteMagazzino *y = x->left;
    x->left = y->right;

    if (y->right != (&magazzino)->NIL)
        y->right->p = x;

    y->p = x->p;

    if (x->p == (&magazzino)->NIL)
        (&magazzino)->root = y;
    else if (x == x->p->right)
        x->p->right = y;
    else
        x->p->left = y;

    y->right = x;
    x->p = y;
}

IngredienteMagazzino * /*bool*/ inserisciInMagazzino(IngredienteMagazzino *root, char *nome_ingrediente, unsigned short quantitaTotale, LottoIngrediente *listaLotti) // Insert in RBTree Magazzino
{
    IngredienteMagazzino *z = (IngredienteMagazzino *)malloc(sizeof(IngredienteMagazzino));
    strncpy(z->nome_ingrediente, nome_ingrediente, MAX_LENGTH_NOME - 1);
    z->quantitaTotale = quantitaTotale;
    z->listaLotti = listaLotti;

    z->colore = RED;
    z->left = (&magazzino)->NIL;
    z->right = (&magazzino)->NIL;
    z->p = (&magazzino)->NIL;

    IngredienteMagazzino *x = (&magazzino)->root;
    IngredienteMagazzino *y = (&magazzino)->NIL;

    while (x != (&magazzino)->NIL)
    {
        y = x;
        if (strcmp(z->nome_ingrediente, x->nome_ingrediente) < 0)
            x = x->left;
        else
            x = x->right;
    }

    z->p = y;

    if (y == (&magazzino)->NIL)
        (&magazzino)->root = z;
    else if (strcmp(z->nome_ingrediente, y->nome_ingrediente) < 0)
        y->left = z;
    else
        y->right = z;

    insertFixupMagazzino(z);

    return z;
}

void insertFixupMagazzino(IngredienteMagazzino *z) // Insert Fix Up RBTree Magazzino
{
    IngredienteMagazzino *y;

    while (z->p->colore == RED)
    {
        if (z->p == z->p->p->left)
        {
            y = z->p->p->right;
            if (y->colore == RED)
            {
                z->p->colore = BLACK;
                y->colore = BLACK;
                z->p->p->colore = RED;
                z = z->p->p;
            }
            else
            {
                if (z == z->p->right)
                {
                    z = z->p;
                    leftRotateMagazzino(z);
                }
                z->p->colore = BLACK;
                z->p->p->colore = RED;
                rightRotateMagazzino(z->p->p);
            }
        }

        else
        {
            y = z->p->p->left;
            if (y->colore == RED)
            {
                z->p->colore = BLACK;
                y->colore = BLACK;
                z->p->p->colore = RED;
                z = z->p->p;
            }
            else
            {
                if (z == z->p->left)
                {
                    z = z->p;
                    rightRotateMagazzino(z);
                }
                z->p->colore = BLACK;
                z->p->p->colore = RED;
                leftRotateMagazzino(z->p->p);
            }
        }
    }
    (&magazzino)->root->colore = BLACK;
}

IngredienteMagazzino *trovaInMagazzino(IngredienteMagazzino *x, char *nome_ingrediente) // ricerca e restituzione Ingrediente in magazzino
{
    if (x == (&magazzino)->NIL || strcmp(x->nome_ingrediente, nome_ingrediente) == 0)
        return x;
    if (strcmp(nome_ingrediente, x->nome_ingrediente) < 0)
        return trovaInMagazzino(x->left, nome_ingrediente);
    else
        return trovaInMagazzino(x->right, nome_ingrediente);
}

void modificaScorte(IngredienteMagazzino *ingrediente, unsigned short quantita_prodotto, int scadenza) // aggiunta o rimozione di quantita_prodotto dall'ingredionete in magazzino e aggiornamento listaLotti
{
    //* HP: caso di rimozione scadenza = -1
    //* HP: già fatto controllo presenza ingrediente (caso aggiunta)
    //* HP: già fatto controllo sufficienza prodotto (caso riduzione)

    if (scadenza < 0) // rimozione
    {
        ingrediente->quantitaTotale -= quantita_prodotto;

        unsigned short da_rimuovere = quantita_prodotto;
        unsigned short quantita_primo_lotto = 0;
        while (da_rimuovere > 0)
        {
            quantita_primo_lotto = ingrediente->listaLotti->quantita;
            if (quantita_primo_lotto <= da_rimuovere)
            {
                da_rimuovere -= quantita_primo_lotto;
                rimuoviPrimoLotto(&(ingrediente->listaLotti));
            }
            else
            {
                quantita_primo_lotto -= da_rimuovere;
                ingrediente->listaLotti->quantita = quantita_primo_lotto;
                da_rimuovere = 0;
            }
        }
    }
    else
    {
        ingrediente->quantitaTotale += quantita_prodotto;
        aggiungiLotto((&(ingrediente->listaLotti)), scadenza, quantita_prodotto);
    }
}

bool ingredientiSufficienti(Ricetta *ricetta, unsigned short quantita) // controllo qunatita ingrediente in magazzino sufficiente per ogni ingrediente di nome_ricetta
{
    IngredienteRicetta *ingrediente = ricetta->ingredientiRicetta;
    unsigned short quantita_utilizzata = 0;

    while (ingrediente != NULL)
    {
        quantita_utilizzata = ingrediente->quantita * quantita;
        if ((ingrediente->ingredienteMagazzino == (&magazzino)->NIL) || (quantita_utilizzata > ingrediente->ingredienteMagazzino->quantitaTotale))
            return false;
        ingrediente = ingrediente->next;
    }
    return true;
}

void controlloScaduti(IngredienteMagazzino *ingrediente) // controllo scadenze lotti degli ingredienti in magazzino
{
    if (ingrediente == (&magazzino)->NIL)
        return;
    else
    {
        controlloScaduti(ingrediente->left);

        while (ingrediente->listaLotti != NULL && (ingrediente->listaLotti->scadenza) < tempo + 1)
        {
            ingrediente->quantitaTotale -= ingrediente->listaLotti->quantita;
            rimuoviPrimoLotto(&ingrediente->listaLotti);
        }

        controlloScaduti(ingrediente->right);
        return;
    }
}

// Gestione Lotti in Magazzino [Lista con Inserimento Ordinato e Rimozione solo dalla testa]
void aggiungiLotto(LottoIngrediente **testa, int scadenza, unsigned short quantita)
{
    LottoIngrediente *nuovoLotto = (LottoIngrediente *)malloc(sizeof(LottoIngrediente));
    nuovoLotto->scadenza = scadenza;
    nuovoLotto->quantita = quantita;
    nuovoLotto->next = NULL;

    if (*testa == NULL || scadenza < (*testa)->scadenza)
    {
        nuovoLotto->next = *testa;
        *testa = nuovoLotto;
        return;
    }

    LottoIngrediente *temp = *testa;
    while (temp->next != NULL && temp->next->scadenza <= scadenza)
    {
        temp = temp->next;
    }

    nuovoLotto->next = temp->next;
    temp->next = nuovoLotto;
}

void rimuoviPrimoLotto(LottoIngrediente **testa)
{
    LottoIngrediente *temp = *testa;
    *testa = (*testa)->next;
    free(temp);
}

void rimuoviListaLotti(LottoIngrediente *listaLotti)
{
    LottoIngrediente *temp;
    while (listaLotti != NULL)
    {
        temp = listaLotti;
        listaLotti = listaLotti->next;
        free(temp);
    }
}

LottoIngrediente *duplicaListaLotti(LottoIngrediente *listaLotti)
{
    if (listaLotti == NULL)
        return NULL;

    LottoIngrediente *nuovaLista = (LottoIngrediente *)malloc(sizeof(LottoIngrediente));
    if (nuovaLista == NULL)
        return NULL;

    nuovaLista->scadenza = listaLotti->scadenza;
    nuovaLista->quantita = listaLotti->quantita;
    nuovaLista->next = duplicaListaLotti(listaLotti->next);

    return nuovaLista;
}

// PROCESSA RIFORNIMENTO
void processaRifornimento()
{
    char nome_ingrediente[MAX_LENGTH_NOME];
    unsigned short quantita;
    int scadenza;

    Er = scanf("%*[^ ]%*c"); // elimino "ggiungi_ricetta"

    do
    {
        Er = scanf("%s", nome_ingrediente);
        Er = scanf("%hu", &quantita);
        Er = scanf("%d", &scadenza);

        if (scadenza > tempo) // evitiamo di inserire lotti già scaduti perchè processeremmo gli ordini in attesa prima di controllare la scadenza
        {
            IngredienteMagazzino *ingrediente = trovaInMagazzino((&magazzino)->root, nome_ingrediente);
            if (ingrediente != (&magazzino)->NIL)
            {
                modificaScorte(ingrediente, quantita, scadenza);
            }
            else
            {
                LottoIngrediente *listaLotti = NULL;
                aggiungiLotto((&listaLotti), scadenza, quantita);
                inserisciInMagazzino((&magazzino)->root, nome_ingrediente, quantita, listaLotti);
            }
        }
    } while (!(getc_unlocked(file) == '\n'));

    processaOrdiniInAttesa();
}

// Gestione Ricettario [BST]
bool aggiungiRicetta()
{
    char nome_ricetta[MAX_LENGTH_NOME];
    unsigned short peso_ricetta = 0;
    char nome_ingrediente[MAX_LENGTH_NOME];
    unsigned short quantita;

    Er = scanf("%*[^ ]%*c");        // elimino "ggiungi_ricetta"
    Er = scanf("%s", nome_ricetta); // prendo <nome_ricetta>

    if (prendiInRicettario(ricettario, nome_ricetta)) // se già abbiamo la ricetta ignoriamo
    {
        Er = scanf("%*[^\n]%*c"); // elimino resto linea e \n
        return false;
    }
    IngredienteRicetta *ingredientiRicetta = NULL;

    while (!(getc_unlocked(file) == '\n'))
    {
        Er = scanf("%s", nome_ingrediente); // prendo <nome_ingrediente>
        Er = scanf("%hu", &quantita);       // prendo <numero_elementi_ordinati>

        inserisciIngredienteRicetta(&ingredientiRicetta, nome_ingrediente, quantita);

        peso_ricetta += quantita;
    }

    inserisciInRicettario(nome_ricetta, peso_ricetta, ingredientiRicetta);
    return true;
}

void rimuoviRicetta()
{
    char nome_ricetta[MAX_LENGTH_NOME];

    Er = scanf("%*[^ ]%*c"); // elimino "imuovi_ricetta"
    Er = scanf("%s", nome_ricetta);
    Er = getc_unlocked(file);

    if (!prendiInRicettario(ricettario, nome_ricetta)) // se non esiste
    {
        printf("non presente\n");
        return;
    }
    else if (presenteInCodaOrdiniPronti(nome_ricetta) || presenteInListaAttesa(nome_ricetta)) // se è tra gli ordini in coda
    {
        printf("ordini in sospeso\n");
        return;
    }
    else // rimozione
    {
        rimuoviDaRicettario(nome_ricetta);
        printf("rimossa\n");
    }
}

// Gestione Ricettario [BST]
Ricetta *prendiInRicettario(Ricetta *x, char *nome_ricetta) // ricerca ricorsiva su BST Cormen
{
    if (x == NULL || !strcmp(x->nome_ricetta, nome_ricetta))
        return x;
    if (strcmp(x->nome_ricetta, nome_ricetta) > 0)
        return prendiInRicettario(x->left, nome_ricetta);
    if (strcmp(x->nome_ricetta, nome_ricetta) < 0)
        return prendiInRicettario(x->right, nome_ricetta);
    return NULL;
}

Ricetta *minRicettario(Ricetta *root) // ricerca min su BST Cormen
{
    while (root->left != NULL)
        root = root->left;
    return root;
}

void transplantRicettario(Ricetta *u, Ricetta *v) // trasplant BST Cormen per la remove
{
    if (u->p == NULL)
        ricettario = v;
    else if (u == u->p->left)
        u->p->left = v;
    else
        u->p->right = v;

    if (v != NULL)
        v->p = u->p;
}

void inserisciInRicettario(char *nome_ricetta, unsigned short peso_ricetta, IngredienteRicetta *ingredientiRicetta) // inserimento BST Cormen
{
    Ricetta *nuovaRicetta = (Ricetta *)malloc(sizeof(Ricetta));
    strncpy(nuovaRicetta->nome_ricetta, nome_ricetta, MAX_LENGTH_NOME - 1);
    nuovaRicetta->peso_ricetta = peso_ricetta;
    nuovaRicetta->ingredientiRicetta = ingredientiRicetta;
    nuovaRicetta->left = NULL;
    nuovaRicetta->right = NULL;
    nuovaRicetta->p = NULL;

    Ricetta *x = ricettario;
    Ricetta *y = NULL;

    while (x != NULL)
    {
        y = x;
        if (strcmp(nuovaRicetta->nome_ricetta, x->nome_ricetta) < 0)
            x = x->left;
        else
            x = x->right;
        nuovaRicetta->p = y;
    }
    if (y == NULL)
        ricettario = nuovaRicetta;
    else if (strcmp(nuovaRicetta->nome_ricetta, y->nome_ricetta) < 0)
        y->left = nuovaRicetta;
    else
        y->right = nuovaRicetta;
}

void rimuoviDaRicettario(char *nome_ricetta) // rimozione BST Cormen
{
    Ricetta *z = prendiInRicettario(ricettario, nome_ricetta);
    Ricetta *y = NULL;

    if (z->left == NULL)
        transplantRicettario(z, z->right);
    else if (z->right == NULL)
        transplantRicettario(z, z->left);
    else
    {
        y = minRicettario(z->right);
        if (y != z->right)
        {
            transplantRicettario(y, y->right);
            y->right = z->right;
            y->right->p = y;
        }
        transplantRicettario(z, y);
        y->left = z->left;
        y->left->p = y;
    }
}

// Gestione Ingredienti in Ricetta [Lista]
void inserisciIngredienteRicetta(IngredienteRicetta **testa, char *nome_ingrediente, unsigned short quantita)
{
    IngredienteRicetta *nuovoIngrediente = (IngredienteRicetta *)malloc(sizeof(IngredienteRicetta));

    nuovoIngrediente->ingredienteMagazzino = trovaInMagazzino((&magazzino)->root, nome_ingrediente);

    if (nuovoIngrediente->ingredienteMagazzino == (&magazzino)->NIL) // se la ricetta ha ingredienti non ancora presenti in magazzino
    {
        LottoIngrediente *listaLotti = NULL;
        nuovoIngrediente->ingredienteMagazzino = inserisciInMagazzino((&magazzino)->root, nome_ingrediente, 0, listaLotti);
    }

    nuovoIngrediente->quantita = quantita;
    nuovoIngrediente->next = NULL;

    if (*testa == NULL)
    {
        *testa = nuovoIngrediente;
        return;
    }

    IngredienteRicetta *ultimoIngrediente = *testa;
    while (ultimoIngrediente->next != NULL)
        ultimoIngrediente = ultimoIngrediente->next;

    ultimoIngrediente->next = nuovoIngrediente;
}

void cancellaListaIngredienti(IngredienteRicetta *listaIngredienti)
{
    IngredienteRicetta *temp;
    while (listaIngredienti != NULL)
    {
        temp = listaIngredienti;
        listaIngredienti = listaIngredienti->next;
        free(temp);
    }
}

// PROCESSA ORDINE
bool processaOrdine()
{
    char nome_ricetta[MAX_LENGTH_NOME];
    unsigned short quantita;

    Er = scanf("%*[^ ]%*c");        // elimino "rdine "
    Er = scanf("%s", nome_ricetta); // prendo <nome_ricetta>
    Er = scanf("%hu", &quantita);   // prendo <numero_elementi_ordinati>
    Er = scanf("%*[^\n]%*c");       // elimino resto linea e \n
    Er = getc_unlocked(file);

    Ricetta *ricetta = prendiInRicettario(ricettario, nome_ricetta);

    if (!ricetta) // non esiste la ricetta
    {
        return false;
    }
    else if (!ingredientiSufficienti(ricetta, quantita)) // non ci sono ingredienti sufficienti
    {
        inserisciInListaAttesa(tempo, ricetta, quantita);
        return true;
    }
    else // esiste la ricetta e ci sono gli ingredienti
    {
        rimuoviIngredientiUtilizzati(ricetta, quantita);
        inserisciInCodaOrdiniPronti(tempo, ricetta, quantita); // inseriamo nelal coda ordini pronti
        return true;
    }
}

void rimuoviIngredientiUtilizzati(Ricetta *ricetta, unsigned short quantita)
{
    IngredienteRicetta *ingrediente = ricetta->ingredientiRicetta;
    unsigned short quantita_utilizzata = 0;

    while (ingrediente != NULL)
    {
        quantita_utilizzata = ingrediente->quantita * quantita;
        modificaScorte(ingrediente->ingredienteMagazzino, quantita_utilizzata, -1);
        ingrediente = ingrediente->next;
    }
}

// Gestione Ordini in Attesa [Lista con inserimento in coda e rimozione su iterazione]
void inserisciInListaAttesa(int tempoOrdine, Ricetta *ricetta, unsigned short quantita)
{
    Ordine *nuovoOrdine = (Ordine *)malloc(sizeof(Ordine));
    if (nuovoOrdine == NULL)
    {
        printf("Errore allocazione in inserisciInListaAttesa");
        return;
    }
    nuovoOrdine = creaOrdine(tempoOrdine, ricetta, quantita, 0);

    if ((&listaAttesa)->testa == NULL)
    {
        (&listaAttesa)->testa = (&listaAttesa)->coda = nuovoOrdine;
    }
    else
    {
        (&listaAttesa)->coda->next = nuovoOrdine;
        (&listaAttesa)->coda = nuovoOrdine;
    }
}

void processaOrdiniInAttesa()
{
    Ordine *ordine = (&listaAttesa)->testa;
    Ordine *prec = NULL;

    while (ordine != NULL)
    {
        if (ingredientiSufficienti(ordine->ricetta, ordine->quantita))
        {
            rimuoviIngredientiUtilizzati(ordine->ricetta, ordine->quantita);
            inserisciOrdinatoInCodaOrdiniPronti(ordine->tempoOrdine, ordine->ricetta, ordine->quantita);
            if (prec == NULL)
            {
                (&listaAttesa)->testa = ordine->next;
                if ((&listaAttesa)->testa == NULL)
                {
                    (&listaAttesa)->coda = NULL;
                }
            }
            else
            {
                prec->next = ordine->next;
                if (ordine->next == NULL)
                {
                    (&listaAttesa)->coda = prec;
                }
            }
            Ordine *temp = ordine;
            ordine = ordine->next;
            free(temp);
        }
        else
        {
            prec = ordine;
            ordine = ordine->next;
        }
    }
}

bool presenteInListaAttesa(char *nome_ricetta)
{
    Ordine *temp = (&listaAttesa)->testa;
    while (temp != NULL)
        if (!strcmp(temp->ricetta->nome_ricetta, nome_ricetta)) // = 0 se sono uguali
            return true;
        else
            temp = temp->next;
    return false;
}

// Gestione Ordini Pronti [Lista con inserimento da coda, inserimento ordinato, rimozione solo da testa ]
Ordine *creaOrdine(int tempoOrdine, Ricetta *ricetta, unsigned short quantita, unsigned short pesoOrdine)
{
    Ordine *nuovoOrdine = (Ordine *)malloc(sizeof(Ordine));
    if (nuovoOrdine == NULL)
    {
        printf("Errore allocazione in creaOrdine");
        return NULL;
    }
    nuovoOrdine->tempoOrdine = tempoOrdine;
    nuovoOrdine->ricetta = ricetta;
    nuovoOrdine->quantita = quantita;
    nuovoOrdine->pesoOrdine = pesoOrdine;
    nuovoOrdine->next = NULL;
    return nuovoOrdine;
}

void inizializzaCodaOrdiniPronti()
{
    (&codaOrdiniPronti)->coda = NULL;
    (&codaOrdiniPronti)->testa = NULL;
}

bool isEmptyCodaOrdiniPronti()
{
    return ((&codaOrdiniPronti)->testa == NULL);
}

void inserisciInCodaOrdiniPronti(int tempoOrdine, Ricetta *ricetta, unsigned short quantita)
{
    unsigned short pesoOrdine = ricetta->peso_ricetta * quantita;
    Ordine *nuovoOrdine = creaOrdine(tempoOrdine, ricetta, quantita, pesoOrdine);
    if (isEmptyCodaOrdiniPronti())
    {
        (&codaOrdiniPronti)->testa = nuovoOrdine;
        (&codaOrdiniPronti)->coda = nuovoOrdine;
    }
    else
    {
        (&codaOrdiniPronti)->coda->next = nuovoOrdine;
        (&codaOrdiniPronti)->coda = nuovoOrdine;
    }
}

void inserisciOrdinatoInCodaOrdiniPronti(int tempoOrdine, Ricetta *ricetta, unsigned short quantita)
{
    unsigned short pesoOrdine = ricetta->peso_ricetta * quantita;
    Ordine *nuovoOrdine = creaOrdine(tempoOrdine, ricetta, quantita, pesoOrdine);

    if (isEmptyCodaOrdiniPronti())
    {
        (&codaOrdiniPronti)->testa = nuovoOrdine;
        (&codaOrdiniPronti)->coda = nuovoOrdine;
    }
    else if ((&codaOrdiniPronti)->testa->tempoOrdine > nuovoOrdine->tempoOrdine)
    {
        nuovoOrdine->next = (&codaOrdiniPronti)->testa;
        (&codaOrdiniPronti)->testa = nuovoOrdine;
    }
    else
    {
        Ordine *current = (&codaOrdiniPronti)->testa;
        while (current->next != NULL && current->next->tempoOrdine < nuovoOrdine->tempoOrdine)
        {
            current = current->next;
        }
        nuovoOrdine->next = current->next;
        current->next = nuovoOrdine;

        if (nuovoOrdine->next == NULL)
        {
            codaOrdiniPronti.coda = nuovoOrdine;
        }
    }
}

Ordine *rimuoviDaCodaOrdiniPronti()
{
    Ordine *primoOrdine = (&codaOrdiniPronti)->testa;
    (&codaOrdiniPronti)->testa = (&codaOrdiniPronti)->testa->next;
    if (!(&codaOrdiniPronti)->testa)
        (&codaOrdiniPronti)->coda = NULL;
    return primoOrdine;
}

bool presenteInCodaOrdiniPronti(char *nome_ricetta)
{
    Ordine *ordineCurr = (&codaOrdiniPronti)->testa;
    while (ordineCurr != NULL)
    {
        if (!strcmp(ordineCurr->ricetta->nome_ricetta, nome_ricetta)) // strcmp restituisce zero se le stringhe sono uguali
            return true;
        ordineCurr = ordineCurr->next;
    }
    return false;
}

// Gestione Corriere
void configuraCorriere()
{
    Er = scanf("%hu", &periodicita); // prendo periodicita
    Er = scanf("%d", &capienza);     // prendo capienza corriere
    Er = scanf("%*[^\n]%*c");        // elimino resto linea
    Er = getc_unlocked(file);
}

void corriCorriCorriere()
{
    long tempCapienza = capienza;
    if (tempo > 0 && tempo % periodicita == 0)
    {
        Ordine *ordine = NULL;
        Ordine *listaCaricamento = NULL;
        bool vuoto = true;

        // creazione lista caricamento
        while (((&codaOrdiniPronti)->testa != NULL) && (tempCapienza >= (&codaOrdiniPronti)->testa->pesoOrdine))
        {
            tempCapienza -= (&codaOrdiniPronti)->testa->pesoOrdine;
            ordine = rimuoviDaCodaOrdiniPronti();
            ordine->next = NULL;
            inserisciInListaCaricamento((&listaCaricamento), ordine);
            vuoto = false;
        }

        // svuotamento lista
        while (listaCaricamento != NULL)
        {
            Ordine *temp = listaCaricamento;
            printf("%d %s %d\n", temp->tempoOrdine, temp->ricetta->nome_ricetta, temp->quantita);
            listaCaricamento = listaCaricamento->next;
            free(temp);
        }

        if (vuoto)
            printf("camioncino vuoto\n");
    }
}

void inserisciInListaCaricamento(Ordine **testa, Ordine *nuovoOrdine)
{
    if (*testa == NULL || nuovoOrdine->pesoOrdine > (*testa)->pesoOrdine)
    {
        nuovoOrdine->next = *testa;
        *testa = nuovoOrdine;
    }
    else
    {
        Ordine *curr = *testa;
        while (curr->next != NULL && ((curr->next->pesoOrdine > nuovoOrdine->pesoOrdine) || ((curr->next->pesoOrdine == nuovoOrdine->pesoOrdine) && (curr->next->tempoOrdine < nuovoOrdine->tempoOrdine))))
        {
            curr = curr->next;
        }
        nuovoOrdine->next = curr->next;
        curr->next = nuovoOrdine;
    }
}
