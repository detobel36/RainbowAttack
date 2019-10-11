# Idées à implémenter/faire/essayer...
- Fichier à accès aléatoire
- :heavy_check_mark: Recherche en triant les tail et en faisant une recherche dicotomique
- Vérifier que la "reverse" fonction est bien répartie (actuellement cela ne semble pas le cas)

# Questions
- Pouvons-nous écrire ce que nous voulons dans le fichier stockant la rainbow table (un nombre au début)
    - Est-ce que le fichier rainbow table doit pouvoir être "exporté" facilement ?  Et donc contenir la taille min-max des mot de passes, le nombre de mot de passe ect...    
   ===> Structure que l'on veut (donc on peut dire que 1 fichier est adapté seulement pour une taille de mdp)
- Autorisation d'utiliser les librairies pour le tri ? (quicksort dans une lib, autre...)
   ===> Oui mais il faut que ça s'installe (ubuntu/debian/linux de base)


# TODO
- Vérifier que la boucle pour hash et reverse plusieurs fois
- :heavy_check_mark: Mettre en place le sytème de tri (différents fichiers)
    - Adapter pour pouvoir gérer des tailles de mots de passe variable (de 6 à 10 par exemple)
- Passer d'un string à un binaire de manière plus efficace
- Supprimer les mots de passe "en double".
- Multi-thread la recherche (qui utilise la rainbowtable)

## Fonctionnement de la recherche

Imaginons un cas avec 3 "colonnes".        

*Tableau*
```
HEAD                                                                 TAIL
pass1_0 -H-> hash1_1 -R1-> pass1_1 -H-> hash1_2 -R2-> pass1_2 -H-> hash1_3 -R3-> pass1_3
pass2_0 -H-> hash2_1 -R1-> pass2_1 -H-> hash2_2 -R2-> pass2_2 -H-> hash2_3 -R3-> pass2_3
```
Si l'on a `pass1_1` en entré.   

*1* On recherche dans les tail. -> Pas trouvé
*2* On applique R3 à H(`pass1_1`) = `pass1_1'` (il pourrait être comparable à `pass?_2`) -> Pas trouvé
*3* On applique R3(H(R2(`pass1_1`))) = `pass1_1` -> Trouvé

Cela signifie que le résultat de l'étape 2 ne peut pas être utilisé pour l'étape 3 et inversement.   
Pour 50k colonnes.  Si le résultat que l'on cherche se trouve dans la "première" colonne, il faudra appliquer 50k fois hash - reduce.  Et l'étape précédente 49 999 fois. Ce qui correspond à la `\sum_{i=0}^{50k}(i)`.



## Méthode reduce
La fonction de hashage envoie sur toutes les lettres et tous les chiffres: [a-z0-9]. Ce qui correspond à 26 lettres et 10 chiffres, ce qui donne 36 possibilités. Les mots de passes utilisés ici sont composés de 62 caractères.   
On ne peut donc pas faire une correspondance 1 <-> 1.  Il est par contre envisageable de faire une correspondance 2 <-> 1.  Avec deux caractères du hash, il sera possible de choisir une nouvelle valeur.  

Pour le hash suivant:
```
d0ab54c4fffe32871b1557d5d424f69391998b98253b3342d394cd29ef58ff5b
```
Il est possible de transformer `d0` en un nombre.  Il faut convertir `d` en entier qui sera multiplié par 36 et additionné a la valeur de `0`. L'index de la fonction de réduction pourra être ajouté et enfin, un modulo permettant de revenir dans l'ensemble des caractères possibles pour les mots de passe que l'on veut générer.


## Méthode de tri

Il n'est pas possible de trier directement tous les mot de passes "tail".  Ils ne rentrent pas tous dans la mémoire (max 12Go).   
**Solution:**    
On garde en mémoire un nombre raisonnable de résultats (disons 1Go). Ces données peuvent être trié facilement. Ils sont ensuite stocké dans un fichier.
Une fois le résultat sauvegardé, on supprime les données présentes en mémoire et on recommence.   
Dans un second temps, on va ouvrir tous les fichiers (si 1Go par fichier, max 12 fichiers) et lire dans l'ordre les éléments. On fera une comparaison des 12 éléments pour prendre le premier et le stocker dans le fichier final.


Exemple concrêt avec 3 fichiers:

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd   <-          bacdd   <-          admbx   <-
    abcdm               duxva               cuamd   
                                                    
                                                    
                        FINAL                       
                                                    
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   <-
    abcdm   <-          duxva               cuamd   
                                                    
                                                    
                        FINAL                       
                        aabcd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   <-
    abcdm               duxva               cuamd   
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd   <-          admbx   
    abcdm               duxva               cuamd   <-
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva   <-          cuamd   <-
    <-                                              
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva   <-          cuamd   
    <-                                      <-      
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
                        cuamd                       
```

--------------

```
    FICHIER_1           FICHIER_2           FICHIER_3
    aabcd               bacdd               admbx   
    abcdm               duxva               cuamd   
    <-                  <-                  <-      
                                                    
                        FINAL                       
                        aabcd                       
                        abcdm                       
                        admbx                       
                        bacdd                       
                        cuamd                       
                        duxva                       
```
