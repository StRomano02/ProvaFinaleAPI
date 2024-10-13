# Progetto (Prova Finale) di Algoritmi e Strutture Dati A.A. 2022/2023 per Ingegneria Informatica al Politecnico di Milano

## Pasticceria Industriale - Simulazione Ordini

Questo progetto simula il funzionamento di una pasticceria industriale che gestisce ordini di dolci, la loro preparazione e la spedizione tramite corriere. L'intera simulazione avviene a tempo discreto, gestendo ingredienti, ricette, ordini, magazzino e consegne con logiche precise e ottimizzate.

---

## Funzionalità principali

- **Gestione degli ingredienti**: La pasticceria stocca diversi ingredienti, ciascuno con una quantità specifica e una data di scadenza.
- **Catalogo delle ricette**: Ogni ricetta è identificata da un nome e specifica le quantità di ciascun ingrediente necessario.
- **Gestione ordini**: Gli ordini dei clienti vengono elaborati in base alla disponibilità degli ingredienti, con ordini in attesa se le scorte sono insufficienti.
- **Rifornimento magazzino**: Il magazzino viene periodicamente rifornito con lotti di ingredienti, che vengono utilizzati seguendo una politica FIFO (First In, First Out).
- **Consegne tramite corriere**: Gli ordini pronti vengono caricati su un camioncino in base alla capienza, priorità cronologica e peso.

---

## Specifiche tecniche

- **Ingredienti**: Ogni ingrediente è identificato da un nome (stringa) e stoccato in lotti con quantità e data di scadenza.
- **Ricette**: Ogni ricetta utilizza specifiche quantità di ingredienti e può essere aggiunta o rimossa dal catalogo. Le ricette rimosse con ordini pendenti non vengono eliminate finché non vengono completati.
- **Ordini**: Gli ordini possono essere fatti tramite una piattaforma online o telefonicamente. Gli ordini vengono elaborati in base alla disponibilità degli ingredienti, con quelli in attesa gestiti cronologicamente.
- **Corriere**: Il corriere ritira gli ordini pronti a intervalli regolari. Gli ordini vengono caricati in ordine di arrivo, ma prioritizzando quelli con maggior peso in caso di capienza insufficiente.

---

## Formato dei comandi

Il file di input segue una serie di comandi per gestire la simulazione:

- `aggiungi_ricetta <nome_ricetta> <nome_ingrediente> <quantità> ...`  
  Aggiunge una nuova ricetta al catalogo.

- `rimuovi_ricetta <nome_ricetta>`  
  Rimuove una ricetta dal catalogo (se non ci sono ordini pendenti per essa).

- `rifornimento <nome_ingrediente> <quantità> <scadenza> ...`  
  Rifornisce il magazzino con nuovi lotti di ingredienti.

- `ordine <nome_ricetta> <numero_elementi_ordinati>`  
  Effettua un nuovo ordine di dolci basato su una ricetta esistente.

---

## Strutture Dati Utilizzate:

- `Albero Rosso-Nero per il Magazzino`  
- `Albero Binario di Ricerca (BST) per il Ricettario`  
- `Liste Dinamiche per la gestione degli Ordini`  



