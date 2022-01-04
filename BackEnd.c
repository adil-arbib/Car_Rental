#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.c"
#include "macros.c"
#ifndef WINDOWSwindowAffClient
#include <windows.h>
#endif

//global variables
HWND windowEmp, windowAjEmp, windowOperation, windowAjClient, windowMdClient, windowAffClient, windowAjVoiture, windowMdVoiture, 
        windowSpVoiture, windowEvaluer, windowLouer, windowStatistique;
HWND hEmpID, hEmpNom, hEmpPrenom, hAjEmpNom, hAjEmpPrenom,
     hCIN,hPermis, hNom, hTel, hVille, hAdresse,
     hNvCIN, hNvTel, hNvVille, hNvAdresse,
     hshowCIN, hShowTel, hShowPermis, hShowAdresse, hShownom, hShowNbrRes,
     hAjMatricule, hAjMarque, hAjModele, 
     hMdMatricule, hModifEtat, hMdayFinAssurance, hMmonthFinAssurance, hMyearFinAssurance,
     hSpMatricule, 
     hLouerCIN, hDateDebut_Jour, hDateDebut_Mois, hDateDebut_annee, hDateFin_Jour, hDateFin_Mois,
     hDateFin_annee,hMarques, hModele, hPrixJour, hPrixTotal, 
     hID, hMarqueEval, hCINEval, hMatrEval, hDateEval_jour, hDateEval_mois, hDateEval_annee, hEvalEtat,
     hStatique_Mois, hStatique_Annee, hPrixMoisS;

int joursDiff;
//les tailles des tableau qu'on a utilise
int num_matrReser, num_modele, nbrTousMatr, nbrAssur, nbrMatrEtat, tailleInter, tailleDiff, nbrModele;


//verifier les entrees
int verifierSiVide(HWND *hwnd, int n){
    char buff[100];
    for(int i=0; i<n; i++){
        GetWindowText(hwnd[i], buff, 100);
        if(strcmp(buff, "")==0)return 0;
    }
    return 1;
}
// effacer le contenent d'une fenetre
void effacerWindow(HWND *hwnds, int n){
    for(int i=0; i<n; i++){
        SetWindowText(hwnds[i], L"");
    }
}

//vérifier les donnees de l'employee
int verifierEmp(HWND *hwnds){
    char EmpTab[3][20];
    for(int i=0; i<3; i++){
        GetWindowText(hwnds[i], EmpTab[i], 20);
    }
    FILE *femp = fopen("BD/Employe/Employe.txt", "r");
    if(!femp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
    }
    char buff[256], IDTmp[5], NomTmp[30], PrenomTmp[30];

    while(!feof(femp)){
        fgets(buff, 256, femp);
        sscanf(buff, "%s%s%s", IDTmp, NomTmp, PrenomTmp);
        if(strcmp(IDTmp, EmpTab[0])==0 && strcmp(NomTmp, EmpTab[1])==0 && strcmp(PrenomTmp, EmpTab[2])==0)return 1;
    }
    return 0;
}
//verifier si CIN existe
int CINExiste(char *winCIN){
        FILE *fp = fopen("BD/Client/Client.txt","r");
        if(!fp){
                MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
                return -1;
        }
        char buff[256], fileCIN[12];
        while(!feof(fp)){
                fgets(buff,256,fp);
                sscanf(buff,"%s", fileCIN);
                if(strcmp(fileCIN, winCIN)==0){
                        return 1;
                }
        }
        return 0;
        fclose(fp);
}
//stocker les donnees de nouveau client
void ajouterClient(HWND *hwnds){
        FILE *fp = fopen("BD/Client/Client.txt", "a");
        if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        }
        char buff[100];
        for(int i=0;i<6;i++){
                GetWindowText(hwnds[i],buff,100);
                switch (i){
                    case 5 :
                        fprintf(fp,"%-25s",buff);
                        break;
                    case 3 :
                        fprintf(fp, "%-15s", "0");
                    default :
                        fprintf(fp,"%-20s",buff);
                        break;   
                }
        }
        fprintf(fp,"\n");
        fclose(fp);
}
//modifier client
void modifierClient(HWND *hwnds){
    FILE *fp = fopen("BD/Client/Client.txt", "r+");
        if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
        }
        //get user input
        char Entry[4][25];
        for(int i=0; i<4; i++){
            GetWindowText(hwnds[i],Entry[i], 25);
        }
        //parcourir le fichier
        char buff[256], Fcin[12];
        while(!feof(fp)){
            fgets(buff, 256, fp);
            sscanf(buff,"%s", Fcin);
            if(strcmp(Fcin,Entry[0])==0){
                fseek(fp, -67,SEEK_CUR);
                fprintf(fp,"%-20s%-20s%-25s",Entry[1],Entry[2],Entry[3]);
                break;
            }
        }
        fclose(fp);
}

// verifier si le client est dans la liste noir (on peut l'utiliser dans la fenetre de reservation)
int enListNoir(){
    char cin[12];
    GetWindowText(hLouerCIN,cin, 12);
    FILE *fp = fopen("BD/Client/ListeNoir.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    char buff[256], Fcin[12];
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s", Fcin);
        if(strcmp(cin, Fcin)==0)return 1;
    }
    fclose(fp);
    return 0;
}

//verifier si le client est fidele
int EstFidele(){
    char WinCIN[12];
    GetWindowText(hLouerCIN, WinCIN, 12);
    FILE *fp = fopen("BD/Client/Client.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    //parcourir le fichier
    char buff[256], FileCIN[12];
    int nbrRes;
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s", FileCIN);
        if(strcmp(WinCIN, FileCIN)==0){
            fseek(fp, -82, SEEK_CUR);
            fscanf(fp, "%d", &nbrRes);
            if(nbrRes<3)return 0;
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

//incrementation de num Reservation de client
void incrementNbrResr(){
    FILE *fp = fopen("BD/Client/Client.txt", "r+");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return ;
    }
    char Wcin[12];
    GetWindowText(hLouerCIN, Wcin, 12);
    char buff[256], Fcin[12], strNbr[4];
    int lastNbr;
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff,"%s", Fcin);
        if(strcmp(Fcin, Wcin)==0){
            fseek(fp, -82, SEEK_CUR);
            fscanf(fp,"%s", strNbr);
            fseek(fp,-strlen(strNbr),SEEK_CUR);
            fprintf(fp,"%d", atoi(strNbr)+1);
            break;
        }
    }
    fclose(fp);
}

//calculer le prix
int Prix(){
    char prJ[10];
    GetWindowText(hPrixJour, prJ, 10);
    int prixJour = atoi(prJ);
    if(EstFidele())return Remise * joursDiff * prixJour;
    return joursDiff * prixJour;
}

//ajouter voiture dans le fichier Voiture.txt et Assurance.txt
void ajouterVoiture(HWND *hwnds){
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "a");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    //get user input
    char Entry[3][30];
    for(int i=0; i<3; i++){
        GetWindowText(hwnds[i], Entry[i], 30);
    }
    //stocker dans le fichier
    // fprintf(fp,"\n");
    fprintf(fp,"\n%-16s%-16s%-20s%-3s", Entry[0], Entry[1], Entry[2], "100");
    
    fclose(fp);

    fp = fopen("BD/Voiture/Assurance.txt", "a");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    fprintf(fp,"%-16s%-10s", Entry[0], "00/00/0000");
    fprintf(fp,"\n");
    fclose(fp);
}


//return 0 si les dates sont incorrectes
int verefierLesDate(HWND *hwnds){
    int tabDate[6];
    char buff[5];
    for(int i=1; i<7; i++){
        GetWindowText(hwnds[i],buff,5);
        tabDate[i-1] = atoi(buff);
    }
    Date d1 = {tabDate[0],tabDate[1],tabDate[2]};
    Date d2 = {tabDate[3],tabDate[4],tabDate[5]};

    if(!verifierDate(d1, d2))return 0;
    joursDiff = diffDates(d1,d2);
    return 1;
}

/*modifier L'etat de voiture(on peut utiliser cette fonction en deux fenetre, fenetre de l'evaluation
et de la modification de voiture, pour cela on passe la matricule comme un parametre)*/
void modifierEtat(char *matricle, char *NvEtat){
    if(strcmp(NvEtat,"")==0)return;
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "r+");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    char buff[256], Fmatr[12];
    int etat;
    while(!feof(fp)){
        fgets(buff,256, fp);
        sscanf(buff, "%s", Fmatr);
        if(strcmp(Fmatr, matricle)==0){
            fseek(fp,-3,SEEK_CUR);
            fprintf(fp, "%-3s", NvEtat);
            break;
        }
    }
    fclose(fp);
}
//modifier Assurance (on appele cette fonction just dans une fenetre-modifier voiture-)
void modifierAssurance(HWND *hwnds){
    char AssuTab[3][6];
    for(int i=2; i<5; i++){
        GetWindowText(hwnds[i], AssuTab[i-2], 6);
    }
    if(strcmp(AssuTab[0],"")==0 || strcmp(AssuTab[1],"")==0 || strcmp(AssuTab[2],"")==0)return;
    FILE *fp = fopen("BD/Voiture/Assurance.txt", "r+");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    char WinMatr[12];
    GetWindowText(hwnds[0], WinMatr, 12);
    char buff[256], Fmatr[12];
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s", Fmatr);
        if(strcmp(Fmatr,WinMatr)==0){
            fseek(fp, -12, SEEK_CUR);
            fprintf(fp, "%s/%s/%s",AssuTab[0], AssuTab[1], AssuTab[2]);
            break;
        }
    }
    fclose(fp);
}

//get cleint name (pour stocker le client dans la liste noir)
char *getClientName(char *CIN){
    FILE *fp = fopen("BD/Client/Client.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    char *nom = (char*)malloc(sizeof(char)*40);
    char buff[256], Fcin[12], Fpermis[15], Fnom[20], Fprenom[20];
    while(!feof(fp)){
        fgets(buff,256, fp);
        sscanf(buff,"%s%s%s%s", Fcin, Fpermis, Fnom, Fprenom);
        if(strcmp(CIN, Fcin)==0){
            strcat(Fnom, " ");
            strcat(Fnom, Fprenom);
            strcpy(nom, Fnom);
            return nom;
        }
    }
}


int getLastID(){
    char marque[20];
    GetWindowText(hMarques, marque, 20);
    char filepath[100];
    sprintf(filepath,"BD/Voiture/Voiture Reserve/%s.txt",marque);

    FILE *fp = fopen(filepath,"r");
    if (!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    char buff[7];
    int ID;
    fseek(fp,-6, SEEK_END);
    fgets(buff, 7, fp);
    sscanf(buff, "%d", &ID);
    fclose(filepath);
    return ID;
}
//get client info
void AfficherClient(){
    FILE *fp = fopen("BD/Client/Client.txt", "r");
    if (!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    char buff[256], WinCin[12], cin[12], permis[12], nom[30], prenom[20], nbr[4], tel[14], ville[20], adrs1[30],adrs2[30];
    GetWindowText(hshowCIN, WinCin, 40);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s%s%s%s%s%s%s%s%s", cin, permis, nom, prenom, nbr, tel, ville, adrs1, adrs2);
        if(strcmp(cin, WinCin) == 0){
            strcat(nom, " ");
            strcat(nom, prenom);
            strcat(adrs1, " ");
            strcat(adrs1, adrs2);
            SetWindowText(hShowPermis, permis);
            SetWindowText(hShownom, nom);
            SetWindowText(hShowNbrRes, nbr);
            SetWindowText(hShowTel, tel);
            SetWindowText(hShowAdresse, adrs1);
            return;
        }
    }
    MessageBox(NULL,TEXT("CIN non trouve"), "erreur", MB_OK| MB_ICONERROR);
}



// stocker le client dans la liste noir
void AjouterListeNoir(){
    FILE *fp = fopen("BD/Client/ListeNoir.txt", "a");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    char cin[12];
    GetWindowText(hCINEval, cin, 12);
    fprintf(fp, "\n");
    fprintf(fp, "%-15s%s", cin, getClientName(cin));
    fclose(fp);
}


//verifier si la matricule est valide
int MatrExiste(char* matricule){
    
    FILE* fp12 = fopen("BD/Voiture/Voiture.txt","r");

    if(!fp12){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    char MatriculeA[20], buff1[256];
    fseek(fp12, 53, SEEK_SET);
    while(!feof(fp12)){
        fgets(buff1,256,fp12);
        sscanf(buff1,"%s",MatriculeA);
        if(strcmp(matricule,MatriculeA)==0) return 1;  
    }
    fclose(fp12);
    return 0;    
}

//supprimer voiture (voiture.txt et assurance.txt)
void suppVoiture(){
    FILE *fp0 = fopen("BD/Voiture/Voiture.txt", "r");
    FILE *fp1 = fopen("BD/Voiture/tmp.txt", "w");
    if(!fp0 || !fp1){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    printf("\ntest\n");
    char WinMatr[12];
    GetWindowText(hSpMatricule,WinMatr, 12);
    char buff[256], matr[12];
    while(!feof(fp0)){
        fgets(buff,256,fp0);
        sscanf(buff, "%s", matr);
        if(strcmp(matr,WinMatr)!=0){
            fputs(buff,fp1);
        }
    }
    fclose(fp0);
    fclose(fp1);
    remove("BD/Voiture/Voiture.txt");
    rename("BD/Voiture/tmp.txt","BD/Voiture/Voiture.txt");

    //delete from "Assurance.txt"
    fp0 = fopen("BD/Voiture/Assurance.txt", "r");
    fp1 = fopen("BD/Voiture/tmp.txt", "w");
    if(!fp0 || !fp1){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    while(!feof(fp0)){
        fgets(buff,256,fp0);
        sscanf(buff,"%s", matr);
        if(strcmp(WinMatr, matr) != 0){
            fputs(buff, fp1);
        }
    }

    fclose(fp0);
    fclose(fp1);
    remove("BD/Voiture/Assurance.txt");
    rename("BD/Voiture/tmp.txt", "BD/Voiture/Assurance.txt");
}


//intersection entre deux tableaux
char **intersectionTab(char **A, char **B, int tailleA, int tailleB){
    tailleInter = 0;
   int MaxtailleIntersection;
   if(tailleA > tailleB){
       MaxtailleIntersection = tailleB; 
   }
   else{
       MaxtailleIntersection = tailleA;
   }
   char **intersection = (char **) malloc(MaxtailleIntersection*sizeof(char*));
   for(int i = 0 ; i<tailleA ; i++){
       for(int j=0; j<tailleB ; j++){
           if(strcmp(A[i],B[j])==0){
               intersection[tailleInter] = (char*)malloc(sizeof(char)*(strlen(A[i])+1));
               strcpy(intersection[tailleInter],A[i]);
               tailleInter++;
           }
       }
   }
   return intersection;
}

char **DifferenceTab(char **A, char **B, int tailleA, int tailleB){
    tailleDiff = 0;
    
   char **diffrence = (char **) malloc(tailleA*sizeof(char*)); 
   for(int i=0; i<tailleA; i++){
       int check = 1;
       for(int j=0; j<tailleB; j++){
           if(strcmp(A[i],B[j])==0)check = 0;
       }
       if(check==1){
           diffrence[tailleDiff] = (char*)malloc(sizeof(char)*(strlen(A[i])+1));
           strcpy(diffrence[tailleDiff], A[i]);
           tailleDiff++;
       }
   }
   return diffrence;
}

// return tous les matricules d'une marque commande par le client
char **TousMatricules(){
    char marqueCommande[20];
    nbrTousMatr = 0;
    GetWindowText(hMarques, marqueCommande, 20);
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    char buff[256], Fmatr[12], Fmarque[20];
    char **tousMatrs = (char **)malloc(sizeof(char*)*Max_Modeles_De_Marque);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s%s", Fmatr, Fmarque);
        if(strcmp(marqueCommande, Fmarque) == 0){
            tousMatrs[nbrTousMatr] = (char*) malloc(sizeof(char)*(strlen(Fmatr)+1));
            strcpy(tousMatrs[nbrTousMatr], Fmatr);
            nbrTousMatr++;
        }
    }
    fclose(fp);
    return tousMatrs;
}

// return tous les matricules du voitures reserve dans une intervalle de temps
char **matriculeDeVoi_reserve(){
    num_matrReser = 0;
    Date d1, d2;
    char marque[40];
    char buff[256],cin[12], matr[12];
    char **matr_voiture_resr = (char**)malloc(sizeof(char*)*20);

    GetWindowText(hMarques, marque, 40);
    HWND dateEntry[6] = {hDateDebut_Jour, hDateDebut_Mois, hDateDebut_annee, hDateFin_Jour, hDateFin_Mois, hDateFin_annee};
    char tabDate[6][5];
    for(int i=0; i<6; i++){
        GetWindowText(dateEntry[i], tabDate[i], 5);
    }
    Date d3 = {atoi(tabDate[0]),atoi(tabDate[1]),atoi(tabDate[2])};
    Date d4 = {atoi(tabDate[3]),atoi(tabDate[4]),atoi(tabDate[5])};

    char filePath[200];
    sprintf(filePath, "BD/Voiture/Voiture Reserve/%s.txt", marque);
    FILE *fp = fopen(filePath, "r");
    if (!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
    }
    fseek(fp, 116, SEEK_SET);
    while(!feof(fp)){
        fgets(buff,256,fp);
        sscanf(buff, "%s%s %d/%d/%d %d/%d/%d", cin, matr, &(d1.jour), &(d1.mois), &(d1.annee),
                                &(d2.jour), &(d2.mois), &(d2.annee));
        if(intersection(d1,d2,d3,d4)){
            matr_voiture_resr[num_matrReser]= (char*) malloc(sizeof(char)*(strlen(matr)+1));
            strcpy(matr_voiture_resr[num_matrReser],matr);
            num_matrReser++;
        }
    }
    fclose(fp);
    return matr_voiture_resr;
}

//return tous les matricules de voitures ont une assurance valide
char **AssuMatr(){
    nbrAssur = 0;
    FILE *fp = fopen("BD/Voiture/Assurance.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    //allouer la memoire pour un tableau de matricules
    char **AssuMatricle = (char **)malloc(sizeof(char*)*Max_Voiture);
    // obtenir l'entrée de client
    char tabdate[3][6];
    GetWindowText(hDateFin_Jour, tabdate[0], 6);
    GetWindowText(hDateFin_Mois, tabdate[1], 6);
    GetWindowText(hDateFin_annee, tabdate[2], 6);
    
    // convertir en entier et le stocker dans un variable de type Date
    Date WinDate = {atoi(tabdate[0]),atoi(tabdate[1]),atoi(tabdate[2])};

    //parcourir le fichier
    char buff[256], Fmatr[12];
    Date Fdate;
    fseek(fp, 28, SEEK_SET);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff, "%s %d/%d/%d", Fmatr, &(Fdate.jour), &(Fdate.mois), &(Fdate.annee));
        if(comparer_dates(WinDate,Fdate)){
            AssuMatricle[nbrAssur] = (char *)malloc(sizeof(char)*(strlen(Fmatr)+1));
            strcpy(AssuMatricle[nbrAssur], Fmatr);
            nbrAssur++;
        }
    }
    fclose(fp);
    return AssuMatricle;
}

//return les matricules du voitures ont une etat >=70
char **EtatMatr(){
    nbrMatrEtat = 0;
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    //allouer la memoire pour un tableau de matricules
    char **matricules = (char **)malloc(sizeof(char*)*100);

    char buff[256], matr[12], marque[20], md1[20], md2[20];
    int etat;

    fseek(fp, 55, SEEK_SET);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff,"%s %s %s%s %d", matr, marque, md1, md2, &etat);
        if(etat>=70){
            matricules[nbrMatrEtat] = (char*)malloc(sizeof(char)*(strlen(matr)+1));
            strcpy(matricules[nbrMatrEtat], matr);
            nbrMatrEtat++;
        }
    }
    fclose(fp);
    return matricules;
}

// elimination de matricules ont (une etat < 70, reserve, assurance ne suffit pas) par l'intersection de tableaux
char **intersectionMatrs(){
    char **tab0, **tab1, **tab2, **matrReser, **tousMatr, **etatMatr, **assuMatr;
    tousMatr = TousMatricules();
    matrReser = matriculeDeVoi_reserve();
    etatMatr = EtatMatr();
    assuMatr = AssuMatr();

    //elimination de matricules reserve
    tab0 = DifferenceTab(tousMatr, matrReser, nbrTousMatr, num_matrReser);
    //elimination de matricules de voitures ont une etat < 70
    tab1 = intersectionTab(etatMatr, tab0, nbrMatrEtat, tailleDiff);
    //elimination de matricules de voitures ont une assurance non valide
    tab2 = intersectionTab(assuMatr, tab1, nbrAssur, tailleInter);
    
    return tab2;
}


// return les modeles correspondant au tableau d'intersection de matricules
char **Modeles(){
    nbrModele = 0;
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return NULL;
    }
    //declaration d'une tableau de matricules valide
    char **modele = (char **)malloc(sizeof(char*)*Max_Modeles_De_Marque);
    

    char **matriculesV = intersectionMatrs();
    char buff[256],matrF[12], marqueF[20], md1[20], md2[20];
    fseek(fp, 55, SEEK_SET);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff,"%s%s %s%s", matrF, marqueF, md1, md2);
        strcat(md1, " ");
        strcat(md1, md2);
        for(int i=0; i<tailleInter; i++){
            if(strcmp(matrF, matriculesV[i])==0){
                modele[nbrModele] = (char*)malloc(sizeof(char)*(strlen(md1) + 1));
                strcpy(modele[nbrModele], md1);
                nbrModele++;
            }
        }
    }
    fclose(fp);
    return modele;
}


// recherche la matricule de voiture commande par le client
char *matriculeDeVoi_Commande(){
    FILE *fp = fopen("BD/Voiture/Voiture.txt","r");
    if(!fp)MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
    char *matricule = (char*)malloc(sizeof(char)*12);
    char **tabMatriculeReserve = matriculeDeVoi_reserve();
    char buff[256];
    //les colonnes de voiture.txt  (voitureInfo[0] = Matricule, voitureInfo[1] = Marque, voitureInfo[2] = Modele)
    char voitureInfo[4][20];
    // variables pour stocker l'entrée de l'utilisateur (la marque et le modele de voiture)
    char marque[20] ,modele[20];
    GetWindowText(hMarques, marque, 20);
    GetWindowText(hModele, modele, 20);
    fseek(fp,57,SEEK_SET);
    while(!feof(fp)){
        fgets(buff, 256, fp);
        sscanf(buff,"%s%s%s%s", voitureInfo[0],voitureInfo[1],voitureInfo[2], voitureInfo[3]);
        strcat(voitureInfo[2], " ");
        strcat(voitureInfo[2], voitureInfo[3]);
        if(strcmp(voitureInfo[1],marque)==0&&strcmp(voitureInfo[2],modele)==0){
            int i=0;
            for(int j=0; j<num_matrReser;j++){
                if(strcmp(voitureInfo[0],tabMatriculeReserve[j])==0) i=1;
            }
            if(i==0)strcpy(matricule,voitureInfo[0]);
        }
    }
    fclose(fp);
    return matricule;
}



int Gain(){
    int gain = 0;
    FILE *fpDacia = fopen("BD/Voiture/Voiture Reserve/Dacia.txt", "r"),
         *fpFiat = fopen("BD/Voiture/Voiture Reserve/Fiat.txt","r"),
         *fpFord = fopen("BD/Voiture/Voiture Reserve/Ford.txt", "r"),
         *fpPeugeot = fopen("BD/Voiture/Voiture Reserve/Peugeot.txt", "r"),
         *fpRenault = fopen("BD/Voiture/Voiture Reserve/Renault.txt", "r");


    if(!fpDacia || !fpFiat || !fpFord || !fpPeugeot || !fpRenault){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    char WinMoisS[3], WinAnneeS[5];
    GetWindowText(hStatique_Mois, WinMoisS, 3);
    GetWindowText(hStatique_Annee, WinAnneeS, 5);

    int WinMois = atoi(WinMoisS), WinAnnee = atoi(WinAnneeS);
    char buff[256], cin[10], matr[12], dfin[20], md1[20], md2[20], prJ[5];
    int prixT;
    Date fileDate;
    fseek(fpDacia, 116, SEEK_SET);
    fseek(fpFiat, 116, SEEK_SET);
    fseek(fpFord, 116, SEEK_SET);
    fseek(fpPeugeot, 116, SEEK_SET);
    fseek(fpRenault, 116, SEEK_SET);

    while(!feof(fpDacia)){
        fgets(buff,256, fpDacia);
        sscanf(buff,"%s%s %d/%d/%d %s%s%s%s %d", cin, matr, &(fileDate.jour), &(fileDate.mois), &(fileDate.annee),
                dfin, md1, md2, prJ, &prixT);
        if(WinMois==fileDate.mois && WinAnnee==fileDate.annee){
            gain = gain + prixT;
        }
    }

    while(!feof(fpFiat)){
        fgets(buff,256, fpFiat);
        sscanf(buff,"%s%s %d/%d/%d %s%s%s%s %d", cin, matr, &(fileDate.jour), &(fileDate.mois), &(fileDate.annee),
                dfin, md1, md2, prJ, &prixT);
        if(WinMois==fileDate.mois && WinAnnee==fileDate.annee){
            gain = gain + prixT;
        }
    }

    while(!feof(fpFord)){
        fgets(buff,256, fpFord);
        sscanf(buff,"%s%s %d/%d/%d %s%s%s%s %d", cin, matr, &(fileDate.jour), &(fileDate.mois), &(fileDate.annee),
                dfin, md1, md2, prJ, &prixT);
        if(WinMois==fileDate.mois && WinAnnee==fileDate.annee){
            gain = gain + prixT;
            
        }
    }

    while(!feof(fpPeugeot)){
        fgets(buff,256, fpPeugeot);
        sscanf(buff,"%s%s %d/%d/%d %s%s%s%s %d", cin, matr, &(fileDate.jour), &(fileDate.mois), &(fileDate.annee),
                dfin, md1, md2, prJ, &prixT);
        if(WinMois==fileDate.mois && WinAnnee==fileDate.annee){
            gain = gain + prixT;
            
        }
    }
  
    while(!feof(fpRenault)){
        fgets(buff,256, fpRenault);
        sscanf(buff,"%s%s %d/%d/%d %s%s%s%s %d", cin, matr, &(fileDate.jour), &(fileDate.mois), &(fileDate.annee),
                dfin, md1, md2, prJ, &prixT);
        if(WinMois==fileDate.mois && WinAnnee==fileDate.annee){
            gain = gain + prixT;
          
        }
    }
    fclose(fpDacia);
    fclose(fpFiat);
    fclose(fpFord);
    fclose(fpPeugeot);
    fclose(fpRenault);
    return gain;
}

int evaluerEtat(){
    char winMatr[12], StrEtat[4];
    GetWindowText(hMatrEval, winMatr, 12);
    GetWindowText(hEvalEtat, StrEtat, 12);
    int WinEtat = atoi(StrEtat);
    FILE *fp = fopen("BD/Voiture/Voiture.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    char buff[256], FileMatr[12], FileMarque[20], FileMd1[20], FileMd2[20];
    int FileEtat;
    while(!feof(fp)){
        fgets(buff,256, fp);
        sscanf(buff,"%s%s%s%s %d", FileMatr, FileMarque, FileMd1, FileMd2, &FileEtat);
        if(strcmp(FileMatr,winMatr)==0){
            if(WinEtat<FileEtat)return 0;
            return 1;
        }
    }
}
//retourner combien de jours de retard
int Retard(HWND *hwnds){
    //stocker date de retour
    char strDateR[3][6];
    for(int i=4; i<7; i++){
        GetWindowText(hwnds[i],strDateR[i-4],6);
    }
    //convertir en entier et le stocker dans variable de type Date
    Date dateRetour = {atoi(strDateR[0]),atoi(strDateR[1]),atoi(strDateR[2])};
    
    //stocker ID de reservation et la marque
    char WinID[10], WinMarque[20];
    GetWindowText(hwnds[0], WinID,10);
    GetWindowText(hwnds[1], WinMarque,20);

    int ID = atoi(WinID);

    //ouvrir le fichier de reservation du la marque entre
    char filepath[100];
    sprintf(filepath,"BD/Voiture/Voiture Reserve/%s.txt", WinMarque);
    FILE *fp = fopen(filepath,"r");

    char buff[256], cin[12], matr[12], dateD[12], md1[20], md2[20], prJ[5], prT[6];
    int FileID;
    Date dateFin;
    while(!feof(fp)){
        fgets(buff,256, fp);
        sscanf(buff,"%s %s %s %d/%d/%d %s%s %s %s %d", cin, matr, dateD, &(dateFin.jour), &(dateFin.mois), &(dateFin.annee),
                                                    md1, md2, prJ, prT,&FileID);
        if(ID ==FileID){
            return diffDates(dateFin, dateRetour);
        }
    }
    fclose(fp);
    return 404;
}

int verifierDateRetour(){
    char WinID[10], WinMarque[20], dateTab[3][6];
    GetWindowText(hID, WinID, 10);
    GetWindowText(hMarqueEval, WinMarque, 20);
    GetWindowText(hDateEval_jour, dateTab[0], 6);
    GetWindowText(hDateEval_mois, dateTab[1], 6);
    GetWindowText(hDateEval_annee, dateTab[2], 6);
    int ID = atoi(WinID);
    Date dateRetour = {atoi(dateTab[0]),atoi(dateTab[1]),atoi(dateTab[2])};

    char filepath[100];
    sprintf(filepath,"BD/Voiture/Voiture Reserve/%s.txt", WinMarque);

    FILE *fp = fopen(filepath,"r");

    char buff[256], cin[12], matr[12], dateD[12], md1[20], md2[20], prJ[5], prT[6];
    int FileID;
    Date dateFin;
    while(!feof(fp)){
        fgets(buff,256, fp);
        sscanf(buff,"%s %s %s %d/%d/%d %s%s %s %s %d", cin, matr, dateD, &(dateFin.jour), &(dateFin.mois), &(dateFin.annee),
                                                    md1, md2, prJ, prT,&FileID);
        if(ID ==FileID){
            if(!comparer_dates_(dateRetour,dateFin))return 0;
            return 1;
        }
    }
}

//le dernier ID de l'employee
int getLastEmpId(){
    FILE *fp = fopen("BD/Employe/Employe.txt", "r");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return -1;
    }
    fseek(fp, -48, SEEK_END);
    int ID;
    fscanf(fp, "%d", &ID);
    return ID;
}

//ajouter un employee
void ajouterEmp(){
    char WinNom[20], WinPrenom[20];
    GetWindowText(hAjEmpNom, WinNom, 20);
    GetWindowText(hAjEmpPrenom, WinPrenom, 20);
    FILE *fp = fopen("BD/Employe/Employe.txt","a");
    if(!fp) return;
    fprintf(fp, "\n%-16d%-16s%-16s", getLastEmpId() + 1, WinNom,WinPrenom);
    fclose(fp);
}

void stockerReservation(HWND *hwnds){
    char marque[20];
    GetWindowText(hMarques,marque, 20);
    char filepath[100];
    char voitureInfo[100];
    sprintf(filepath,"BD/Voiture/Voiture Reserve/%s.txt",marque);
    FILE *fp = fopen(filepath, "a");
    if(!fp){
        MessageBox(NULL,TEXT("erreur"), "erreur", MB_OK| MB_ICONERROR);
        return;
    }
    //store user input
    char input[11][30];
    for(int i=0; i<11; i++){
        GetWindowText(hwnds[i], input[i], 30);
    }
    fprintf(fp,"\n%-9s%-14s%s/%s/%-12s%s/%s/%-11s%-15s%-15s%-16s%-6d", input[0], matriculeDeVoi_Commande(),
                        input[1], input[2], input[3], input[4], input[5], input[6], input[8], input[9],
                        input[10], getLastID()+1);

    fclose(fp);
}