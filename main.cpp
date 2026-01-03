#include <iostream>
#include <vector>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <fstream>


using namespace std;

void clearScreen () {
    cout << "\033[H\033[2J";
}

const unsigned KReset   (0);
const unsigned KNoir    (40);
const unsigned KRouge   (41);
const unsigned KVert    (42);
const unsigned KJaune   (43);
const unsigned KBleu    (44);
const unsigned KMagenta (45);
const unsigned KCyan    (46);

void couleur (const unsigned & coul) {
    cout << "\033[" << coul <<"m";
}

typedef vector<unsigned> line;
typedef vector<line>      mat;
typedef pair<unsigned,unsigned> position;
//position d'un case
struct maPosition {
    unsigned abs;
    unsigned ord;
};
const unsigned KNbCandies = 6;
//case vide
const unsigned KImpossible = 0;

//affiche la grille du jeu
void displayGrid(const mat & grid) {
    clearScreen();
    couleur(KReset);

    for (const line & row : grid) {
        for (unsigned cel : row) {

            if (cel < 1 || cel > KNbCandies) {
                couleur(KNoir);
                cout << setw(3) << "."; //case vide affiché par un point
                couleur(KReset);
            }
            else {
                switch(cel) {
                case 1: couleur(KRouge);   break;
                case 2: couleur(KVert);    break;
                case 3: couleur(KJaune);   break;
                case 4: couleur(KBleu);    break;
                case 5: couleur(KMagenta); break;
                case 6: couleur(KCyan);    break;
                }

                cout << setw(3) << cel;
                couleur(KReset);
            }
        }
        cout << endl;
    }

    cout << endl;
}

//initialise la grille avec des valeurs
void initGrid(mat & grid, const size_t & taille) {
    grid.resize(taille);
    for (size_t i = 0; i < taille; ++i) {
        grid[i].resize(taille);
        for (size_t j = 0; j < taille; ++j) {
            grid[i][j] = (rand() % KNbCandies) + 1;
        }
    }
}


//Effectue un déplacement de valeur dans la grille selon la position de la valeur et la direction souhaitée
void makeAMove (mat & grid, const maPosition & pos, const char & direction){
    unsigned i = pos.abs;
    unsigned j = pos.ord;
    unsigned n = grid.size();
    if (i>=n || j>= n) return;
    if ((direction == 'Z' || direction == 'z') && i > 0){
        swap(grid[i][j], grid[i-1][j]);
    }
    else if ((direction =='S' || direction == 's') && i < n-1){
        swap(grid[i][j], grid[i+1][j]);
    }
    else if ((direction =='Q' || direction == 'q') && j > 0){
        swap(grid[i][j], grid[i][j-1]);
    }
    else if ((direction =='D' || direction == 'd') && j < n-1){
        swap(grid[i][j], grid[i][j+1]);
    }
}

//Parcours chaque colonne de la grille pour savoir si il y a au moins 3 valeur identiques
bool atLeastThreeInAColumn (const mat &grid, maPosition & pos, unsigned & howMany){
    unsigned n = grid.size();

    for (unsigned j = 0; j < n; ++j) {
        for (unsigned i = 0; i < n ; ++i){
            unsigned value = grid[i][j];
            if (value != KImpossible){
                unsigned count = 1;
                unsigned k = i + 1;
                while (k < n && grid[k][j] == value){
                    count = count + 1;
                    k = k + 1;
                }
                if (count >= 3){
                    pos.abs = i;
                    pos.ord = j;
                    howMany = count;
                    return true;
                }
                i = k - 1;
            }
        }
    }
    return false;
}

//Même principe pour atLeastThreeInAColumn mais vérifie pour les lignes
bool atLeastThreeInARow (const mat &grid, maPosition & pos, unsigned & howMany){
    unsigned n = grid.size();

    for (unsigned i = 0; i < n; ++i) {
        for (unsigned j = 0; j < n ; ++j){
            unsigned value = grid[i][j];
            if (value != KImpossible){
                unsigned count = 1;
                unsigned k = j + 1;
                while (k < n && grid[i][k] == value){
                    count = count + 1;
                    k = k + 1;
                }
                if (count >= 3){
                    pos.abs = i;
                    pos.ord = j;
                    howMany = count;
                    return true;
                }
                j = k - 1;
            }
        }
    }
    return false;
}

//Supprime un alignement vertical et remplace les cases par des KImpossible puis fais une simulation de gravité
void removalInColumn (mat &grid, const maPosition &pos, unsigned howMany) {
    unsigned col = pos.ord;

    for (unsigned i = pos.abs; i < pos.abs + howMany; ++i){
        grid[i][col] = KImpossible;
    }

    for (unsigned i = pos.abs + howMany; i < grid.size(); ++i) {
        unsigned k = i;
        while (k > 0 && grid[k - 1][col] == KImpossible) {
            swap(grid[k][col], grid[k - 1][col]);
            k = k - 1;
        }
    }
}

//Même principe que removalInColumn mais pour les lignes en réutilisant removalInColumn
void removalInRow(mat & grid, const maPosition & pos, unsigned howMany){
    for (unsigned j = 0; j < howMany; ++j) {
        maPosition p;
        p.abs = pos.abs;
        p.ord = pos.ord + j;

        removalInColumn(grid, p, 1);
    }
}

//Menu principal qui permet la séléction du mode de jeux que on veux jouer
unsigned menuPrincipal(){
    unsigned choix;

    clearScreen();
    cout << "Mode de Jeu" << endl;
    cout << "1. Mode Classique" << endl;
    cout << "2. Mode Classé" << endl;
    cout << "3. Mode 1vs1" << endl;
    cout << "4. Mode Histoire" << endl;
    cout << "Choix du mode (1, 2, 3 ...) : ";
    cin >> choix;

    return choix;
}

//Menu de sélection de difficulté pour le mode classique
unsigned choisirTailleGrille(){
    unsigned choix;

    clearScreen();
    cout << "Choix de la Difficulté" << endl;
    cout << "1. Facile (5x5)" << endl;
    cout << "2. Normal (7x7)" << endl;
    cout << "3. Difficile (9x9)" << endl;
    cout << "Choix de la difficulté(1, 2, 3) : ";
    cin >> choix;

    switch(choix){
    case 1:
        return 5; //Retourne la taille de la grille pour plus de lisibilté
    case 2:
        return 7; //Retourne la taille de la grille pour plus de lisibilté
    case 3:
        return 9; //Retourne la taille de la grille pour plus de lisibilté
    default:
        return 5;
    }
}

//Permet de lire le fichier "score.txt" ou est stocké le meilleur score pour le mode classé
unsigned lireMeilleurScore(){
    ifstream fichier ("score.txt");
    unsigned score = 0;

    if (fichier) {
        fichier >> score;
    }

    return score;
}

//Permet de remplacer l'ancien meilleur score par le nouveau meilleur score
void sauvegarderMeilleurScore(unsigned int score) {
    ofstream fichier("score.txt");
    fichier<< score;
}


int main() {
    //Permet d'avoir une grille aléatoire
    srand(time(0));
    //Varaibles global
    unsigned taille;
    unsigned maxCoups;
    unsigned mode = menuPrincipal();
    char direction;
    //Variables lié au score
    unsigned score = 0;
    unsigned scoreJ1 = 0;
    unsigned scoreJ2 = 0;

    unsigned joueurActuel = 1;
    //Grille
    mat grid;
    maPosition pos;

    //Choix du mode de jeu
    if (mode == 1 ){
        taille= choisirTailleGrille(); //Choix de la difficulté du mode classique
        if (taille == 5){ //Facile
            maxCoups =10;
        }
        else if (taille==7){ //Normal
            maxCoups = 15;
        }
        else {
            maxCoups = 20; //Difficile
        }
    }
    else if (mode == 2){
        taille = 7;
        maxCoups = 15;
    }
    else if (mode == 3){
        taille = 7;
        maxCoups = 20;
    }
    else if (mode ==4){ //Mode histoire

        //Niveau 1
        taille = 5;
        maxCoups = 5;
        score = 0;

        clearScreen();
        cout << "Niveau 1" << endl;
        cout << "Bonjour jeune chevalier le royaume est attaqué par des bonbons maléfique" << endl;
        cout << "éliminez 10 bonbons pour avancer" << endl;
        cout << "Appuyez sur entree pour continuer";
        cin.ignore();
        cin.get();

        initGrid(grid, taille);

        for (unsigned coup = 1; coup <= maxCoups; ++coup) {
            displayGrid(grid);

            cout << "Coup " << coup << "/" << maxCoups << endl;
            cout << "Score :" << score << endl << endl;

            cout << "Ligne (0-" << taille-1<<") : ";
            cin >> pos.abs;

            cout << "Colonne (0-" << taille-1<<") : ";
            cin >> pos.ord;

            cout << "Deplacement (z=haut, s=bas, q=gauche, d=droite) : ";
            cin >> direction;

            makeAMove(grid, pos, direction);

            displayGrid(grid);

            unsigned howMany;

            if(atLeastThreeInAColumn(grid, pos, howMany)){
                cout << "Alignement vertical détecté !" << endl;
                removalInColumn(grid, pos, howMany);
                score = score + howMany;
            }
            else if(atLeastThreeInARow(grid, pos, howMany)){
                cout << "Alignement horizontal détecté !" << endl;
                removalInRow(grid, pos, howMany);
                score = score + howMany;
            }

            displayGrid(grid);
        }

        if (score < 10){
            cout << "Vous n'avez pas réussi à sauver le royaume" << endl;
            return 0;
        }
        else {
            cout << "Merci jeune chevalier si vous voulez combattre les force du mal passer par la forêt juste là-bas" << endl;
        }

        //Niveau 2
        taille = 7;
        maxCoups = 10;
        score = 0;

        clearScreen();
        cout << "Niveau 2" << endl;
        cout << "Vous entrez dans la forêt quand soudain un essaim de bonbons maléfiques " << endl << "vous sautent dessus !" << endl;
        cout << "éliminez 20 bonbons pour avancer" << endl;
        cout << "Appuyez sur entree pour continuer"<< endl;
        cin.ignore();
        cin.get();

        initGrid(grid, taille);

        for (unsigned coup = 1; coup <= maxCoups; ++coup) {
            displayGrid(grid);

            cout << "Coup " << coup << "/" << maxCoups << endl;
            cout << "Score :" << score << endl << endl;

            cout << "Ligne (0-" << taille-1<<") : ";
            cin >> pos.abs;

            cout << "Colonne (0-" << taille-1<<") : ";
            cin >> pos.ord;

            cout << "Deplacement (z=haut, s=bas, q=gauche, d=droite) : ";
            cin >> direction;

            makeAMove(grid, pos, direction);

            displayGrid(grid);

            unsigned howMany;

            if(atLeastThreeInAColumn(grid, pos, howMany)){
                cout << "Alignement vertical détecté !" << endl;
                removalInColumn(grid, pos, howMany);
                score = score + howMany;
            }
            else if(atLeastThreeInARow(grid, pos, howMany)){
                cout << "Alignement horizontal détecté !" << endl;
                removalInRow(grid, pos, howMany);
                score = score + howMany;
            }

            displayGrid(grid);
        }

        if (score < 20){
            cout << "Vous n'avez pas réussi à survivre à l'horde de bonbons" << endl;
            return 0;
        }
        else {
            cout << "Vous avez battu ces bonbons vous n'êtes qu'à quelque pas du château maléfique" << endl;
        }

        //Niveau 3
        taille = 9;
        maxCoups = 15;
        score = 0;

        clearScreen();
        cout << "Niveau 3" << endl;
        cout << "Vous êtes dans la salle du trône du méchant Roi réglisse qui veut envahir le royaume"<< endl << "combattez le pour instaurer la paix" << endl;
        cout << "éliminez 30 bonbons pour anéantir les forces du mal" << endl;
        cin.ignore();
        cin.get();

        initGrid(grid, taille);

        for (unsigned coup = 1; coup <= maxCoups; ++coup) {
            displayGrid(grid);

            cout << "Coup " << coup << "/" << maxCoups << endl;
            cout << "Score :" << score << endl << endl;

            cout << "Ligne (0-" << taille-1<<") : ";
            cin >> pos.abs;

            cout << "Colonne (0-" << taille-1<<") : ";
            cin >> pos.ord;

            cout << "Deplacement (z=haut, s=bas, q=gauche, d=droite) : ";
            cin >> direction;

            makeAMove(grid, pos, direction);

            displayGrid(grid);

            unsigned howMany;

            if(atLeastThreeInAColumn(grid, pos, howMany)){
                cout << "Alignement vertical détecté !" << endl;
                removalInColumn(grid, pos, howMany);
                score = score + howMany;
            }
            else if(atLeastThreeInARow(grid, pos, howMany)){
                cout << "Alignement horizontal détecté !" << endl;
                removalInRow(grid, pos, howMany);
                score = score + howMany;
            }

            displayGrid(grid);
        }

        if (score < 30){
            cout << "Vous n'avez pas réussi à vaincre le Roi réglisse" << endl;
            return 0;
        }
        else{
            cout << "Bravo vous avez vaincu les forces du mal félicitations en récompense la princesse " <<endl<< "du royaume vous donne un ticket restaurant périmé !" << endl;
            return 0;
        }
    }


    initGrid(grid, taille);
    //Boucle principal de jeu (quasiment pareil pour le mode histoire)
    for (unsigned coup = 1; coup <= maxCoups; ++coup) {
        displayGrid(grid);

        cout << "Coup " << coup << "/" << maxCoups << endl;
        if (mode == 3){ //Selon le mode de jeu on affiche les scores des deux joueurs ou du joueur
            cout << "Tour du joueur : " << joueurActuel << endl;
            cout << "Score du joueur 1 : " << scoreJ1 << " / Score joueur 2 : " << scoreJ2 << endl;
        }
        else{
            cout << "Score : " << score << endl;
        }

        cout << "Ligne (0-" << taille-1<<") : ";
        cin >> pos.abs;

        cout << "Colonne (0-" << taille-1<<") : ";
        cin >> pos.ord;

        cout << "Deplacement (z=haut, s=bas, q=gauche, d=droite) : ";
        cin >> direction;

        makeAMove(grid, pos, direction);

        displayGrid(grid);

        unsigned howMany;
        bool alignement = false;

        if(atLeastThreeInAColumn(grid, pos, howMany)){
            cout << "Alignement vertical détecté !" << endl;
            removalInColumn(grid, pos, howMany);
            alignement = true;
        }
        else if(atLeastThreeInARow(grid, pos, howMany)){
            cout << "Alignement horizontal détecté !" << endl;
            removalInRow(grid, pos, howMany);
            alignement = true;
        }
        else{
            alignement = false;
        }

        //Permet de faire une fois l'ajout des points selon les deux modes
        if (alignement){
            if (mode == 3){ //Si mode 1vs1 on ajoute au joueur qui a joué
                if (joueurActuel == 1){
                    scoreJ1 = scoreJ1 + howMany;
                }
                else{
                    scoreJ2 = scoreJ2 + howMany;
                }
            }
            else {
                score = score + howMany; //Sinon on ajoute au joueur global (classique ou classé)
            }
        }
        else{
            cout << "Aucun alignement." << endl;
        }

        //Changement du tour du joueur pour le mode 1vs1
        if(mode == 3){
            if (joueurActuel == 1){
                joueurActuel = 2;
            }
            else{
                joueurActuel = 1;
            }
        }

        displayGrid(grid);
        cout << "Appuyez sur entree pour continuer";
        cin.ignore();
        cin.get();
    }
    displayGrid(grid);

    cout << "Partie terminée !" << endl;
    //Affichage des score selon le mode de jeux
    if(mode ==3){
        cout << "Score joueur 1 : " << scoreJ1 << endl; //Affiche le score du joueur 1
        cout << "Score joueur 2 : " << scoreJ2 << endl; //Affiche le score du joueur 2
        //Vérifie qui des deux joueurs à remporter la partie
        if (scoreJ1 > scoreJ2){
            cout << "Victoire du joueur 1 !" << endl;
        }
        else if (scoreJ1 < scoreJ2){
            cout << "Victoire du joueur 2 !" << endl;
        }
        else{
            cout << "égalité" << endl;
        }
    }
    else{
        cout << "Score Final : " << score << endl; //Affichage du score pour le mode classique ou le mode classé
    }

    //Pour le mode classé on vérifie le meilleur score enregistrée
    if (mode ==2 ) {
        unsigned bestScore = lireMeilleurScore();
        if (score > bestScore){
            cout << "Nouveau Record !" << endl;
            sauvegarderMeilleurScore(score);
        }
        else {
            cout << "Record actuel : " << bestScore << endl;
            cout << "Un problème de skills ?" << endl;
        }
    }
    return 0;
}
