#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

void choose(void);
void file_input(void);
void file_output(void);
void memory_free(void);
void scaling_SnH(void);
void scaling_SnH_again(void);
void scaling_bi(void);
void scaling_bi_again(void);
void make_LPF(void);
void LPF_x(int,int,unsigned char*);
void LPF_y(int,int,unsigned char*);
void mse(void);
void finish(void);

// 필요 상수 정의
#define CON 20
#define WIDTH 512
#define HEIGHT 512
#define FILTER_SIZE 7

// 필요 전역 변수 선언
unsigned char* img = NULL;
unsigned char* LPF_img = NULL;
unsigned char* modified_img = NULL;
unsigned char* modified_img2 = NULL;
char *name = NULL;
char *path_in = NULL;
char *path_out = NULL;
double *filter = NULL;
int o_width, o_height, new_width, new_height, choice;

//I11.img가 그나마 괜춘
int main(void){
    make_LPF();
    choose();
    file_output();
    mse();
    memory_free();
    finish();
    return 0;
}

void file_input(void){
    img = (unsigned char*)malloc(sizeof(unsigned char)*WIDTH*HEIGHT);
    name = (char *)malloc(CON*sizeof(char));
    path_in = (char *)malloc(CON*sizeof(char));
    
    if(img == NULL || name == NULL || path_in == NULL){
        printf("ERROR OCCURED\n");
        return;
    }

    FILE* file_in = NULL;
    printf("enter file name without file extension: ");
    scanf("%s",name);
    
    strcpy(path_in, ".\\img\\");
    strcat(path_in,name);
    strcat(path_in,".img");

    file_in = fopen(path_in,"rb");
    fread(img, sizeof(unsigned char), WIDTH*HEIGHT, file_in);
    fclose(file_in);
    printf("file_input done!\n");
}
void file_output(void){
    FILE* file_out = NULL;
    path_out = (char *)malloc(CON*sizeof(char));
    strcpy(path_out, ".\\out\\");
    strcat(path_out,name);
    if(choice == 1) strcat(path_out,"_SnH");
    if(choice == 2) strcat(path_out,"_Bi");
    strcat(path_out,".img");
    file_out = fopen(path_out,"wb");
    fwrite(modified_img2, sizeof(unsigned char), new_width*new_height, file_out);
    fclose(file_out);
    printf("file_output done!\n");
}
void memory_free(void){
    free(img);
    free(name);
    free(modified_img);
    free(modified_img2);
    free(LPF_img);
    free(path_in);
    free(path_out);
    free(filter);
    printf("memory_free done!\n");
}
void finish(void){
   printf("Press any key & enter to exit...\n");
    fflush(stdout); // 출력 버퍼를 즉시 비우기 위해 사용
    while (getchar() == '\n'){} // 아무 입력이 있을 때까지 대기
    printf("Program exited.\n");
}
void scaling_SnH(void){
    printf("\nPerform SnH scaling with LPF.\nEnter the width & height: ");
    scanf("%d %d", &new_width, &new_height);

    unsigned char* snh_x = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
    modified_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);
    modified_img2 = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);

    double delta_w = (double)WIDTH / new_width;
    double delta_h = (double)HEIGHT / new_height;

    // x-axis scaling
    if(new_width<WIDTH) {
        printf("Apply x-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_x(WIDTH,HEIGHT,img);
        else memcpy(LPF_img, img, sizeof(unsigned char)*WIDTH*HEIGHT);
    } else{
        memcpy(LPF_img, img, sizeof(unsigned char)*WIDTH*HEIGHT);
    }
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < new_width; j++){
            int scale_x1 = (int)(j * delta_w);
            int num = i * WIDTH + scale_x1;
            *(snh_x + i * new_width + j) = *(LPF_img + num);
        }
    }

    // y-axis scaling
    if(new_height<HEIGHT){
        printf("Apply y-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_y(new_width,HEIGHT,snh_x);
        else{
            free(LPF_img);
            LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
            memcpy(LPF_img,snh_x,sizeof(unsigned char)*new_width*HEIGHT);
        }
    } else{
        free(LPF_img);
        LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
        memcpy(LPF_img,snh_x,sizeof(unsigned char)*new_width*HEIGHT);
    }
    for(int i = 0; i < new_height; i++){
        for(int j = 0; j < new_width; j++){
            int scale_y1 = (int)(i * delta_h);
            int num = scale_y1 * new_width + j;
            *(modified_img + i * new_width + j) = *(LPF_img + num);
            *(modified_img2 + i * new_width + j) = *(LPF_img + num);
        }
    }

    free(snh_x);
}
void scaling_SnH_again(void){
    int tmp_width = new_width, tmp_height = new_height;
    printf("\nPerform SnH scaling with LPF.\nEnter the width & height: ");
    scanf("%d %d", &new_width, &new_height);

    unsigned char* snh_x = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * tmp_width * tmp_height);
    modified_img2 = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);

    double delta_w = (double)tmp_width / new_width;
    double delta_h = (double)tmp_height / new_height;

    // x-axis scaling
    if(new_width<tmp_width) {
        printf("Apply x-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_x(tmp_width,tmp_height,modified_img);
        else memcpy(LPF_img, modified_img, sizeof(unsigned char)*tmp_width*tmp_height);
    } else{
        memcpy(LPF_img, modified_img, sizeof(unsigned char)*tmp_width*tmp_height);
    }
    for(int i = 0; i < tmp_height; i++){
        for(int j = 0; j < new_width; j++){
            int scale_x2 = (int)(j * delta_w);
            int num = i * tmp_width + scale_x2;
            *(snh_x + i * new_width + j) = *(LPF_img + num);
        }
    }

    // y-axis scaling
    if(new_height<tmp_height){
        printf("Apply y-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_y(new_width,tmp_height,snh_x);
        else{
            free(LPF_img);
            LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
            memcpy(LPF_img,snh_x,sizeof(unsigned char)*new_width*tmp_height);
        }
    } else{
        free(LPF_img);
        LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
        memcpy(LPF_img,snh_x,sizeof(unsigned char)*new_width*tmp_height);
    }
    for(int i = 0; i < new_height; i++){
        for(int j = 0; j < new_width; j++){
            int scale_y2 = (int)(i * delta_h);
            int num = scale_y2 * new_width + j;
            *(modified_img2 + i * new_width + j) = *(LPF_img + num);
        }
    }

    free(snh_x);
}
void scaling_bi(void){
    printf("\nPerform bilinear scaling.\nEnter the width & height: ");
    scanf("%d %d", &new_width, &new_height);
    unsigned char* bi_x = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
    modified_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);
    modified_img2 = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);

    double delta_w = (double)(WIDTH) / new_width;
    double delta_h = (double)(HEIGHT) / new_height;
    // x-axis scaling
    if(new_width<WIDTH) {
        printf("Apply x-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_x(WIDTH,HEIGHT,img);
        else memcpy(LPF_img, img, sizeof(unsigned char)*WIDTH*HEIGHT);
    } else{
        memcpy(LPF_img, img, sizeof(unsigned char)*WIDTH*HEIGHT);
    }
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < new_width; j++){
            double sum=0;
            int scale_x1 = (int)(j*delta_w);
            double diff_x1 = (j*delta_w)-(int)(j*delta_w);
            int num_x = i * WIDTH + scale_x1;
            sum = *(LPF_img+num_x)*(1-diff_x1)+*(LPF_img+num_x+1)*(diff_x1);
            *(bi_x + i * new_width + j) = (unsigned char)sum;
        }
    }
    // y-axis scaling
    if(new_height<HEIGHT){
        printf("Apply y-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_y(new_width,HEIGHT,bi_x);
        else{
            free(LPF_img);
            LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
            memcpy(LPF_img,bi_x,sizeof(unsigned char)*new_width*HEIGHT);
        }
    } else{
        free(LPF_img);
        LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * HEIGHT);
        memcpy(LPF_img,bi_x,sizeof(unsigned char)*new_width*HEIGHT);
    }
    for(int i = 0; i < new_height; i++){
        for(int j = 0; j < new_width; j++){
            double sum=0;
            int scale_y1 = (int)(i * delta_h);
            double diff_y1 = (i * delta_h) - (int)(i * delta_h);
            int num_y = scale_y1 * new_width + j;
            sum = *(LPF_img + num_y) * (1 - diff_y1) + *(LPF_img + num_y + new_width) * diff_y1;
            *(modified_img + i * new_width + j) = (unsigned char)sum;
            sum = *(LPF_img + num_y) * (1 - diff_y1) + *(LPF_img + num_y + new_width) * diff_y1;
            *(modified_img2 + i * new_width + j) = sum;
        }
    }
    free(bi_x);
}
void scaling_bi_again(void){
    free(LPF_img);
    int tmp_width = new_width, tmp_height = new_height;
    printf("\nPerform bilinear scaling.\nEnter the width & height: ");
    scanf("%d %d", &new_width, &new_height);
    unsigned char* bi_x2 = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * tmp_width * tmp_height);
    modified_img2 = (unsigned char*)malloc(sizeof(unsigned char) * new_width * new_height);
    double delta_w = (double)(tmp_width) / new_width;
    double delta_h = (double)(tmp_height) / new_height;
    
    //x-axis scaling
    if(new_width<tmp_width){
        printf("Apply x-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_x(tmp_width, tmp_height,modified_img);
        else memcpy(LPF_img, modified_img, sizeof(unsigned char)*tmp_width*tmp_height);
    } else{
        memcpy(LPF_img, modified_img, sizeof(unsigned char)*tmp_width*tmp_height);
    }
    for(int i = 0; i < tmp_height; i++){
        for(int j = 0; j < new_width; j++){
            double sum=0;
            int scale_x1 = (int)(j*delta_w);
            double diff_x1 = (j*delta_w)-(int)(j*delta_w);
            int num_x = i * tmp_width + scale_x1;
            sum = *(LPF_img+num_x)*(1-diff_x1)+*(LPF_img+num_x+1)*(diff_x1);
            *(bi_x2 + i * new_width + j) = (unsigned char)sum;
        }
    }
    //y-axis scaling
    if(new_height<tmp_height){
        printf("Apply y-axis LPF? Y/N\n");
        char c;
        scanf(" %c",&c);
        if(c=='Y'||c=='y') LPF_y(new_width, tmp_height,bi_x2);
        else{
            free(LPF_img);
            LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
            memcpy(LPF_img,bi_x2,sizeof(unsigned char)*new_width*tmp_height);
        }
    } else{
        free(LPF_img);
        LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * new_width * tmp_height);
        memcpy(LPF_img,bi_x2,sizeof(unsigned char)*new_width*tmp_height);
    }
    for(int i = 0; i < new_height; i++){
        for(int j = 0; j < new_width; j++){
            double sum = 0;
            int scale_y1 = (int)(i * delta_h);
            double diff_y1 = (i * delta_h) - (int)(i * delta_h);
            int num_y = scale_y1 * new_width + j;
            sum = *(LPF_img + num_y) * (1 - diff_y1) + *(LPF_img + num_y + new_width) * diff_y1;
            *(modified_img2 + i * new_width + j) = (unsigned char)sum;
        }
    }
    free(bi_x2);
}
void mse(void){
    double mse = 0;
    for(int i=0;i<HEIGHT;i++){
        for(int j=0;j<WIDTH;j++){
            mse+=pow(*(img+i*WIDTH+j)-*(modified_img2+i*WIDTH+j),2);
        }
    }
    mse /= (double)(WIDTH*HEIGHT);
    printf("mse: %f\n",mse);
}
void choose(void){
    printf("1. Sample & Hold \t2. bilinear\n");
    scanf("%d",&choice);
    switch (choice){
        case 1:
            file_input();
            scaling_SnH();
            printf("One more SnH scaling? Y/N\n");
            char c;
            scanf(" %c",&c);
            if(c=='Y'||c=='y') scaling_SnH_again();
            printf("Sample & Hold scaling done!\n");
            break;
        case 2:
            file_input();
            scaling_bi();
            printf("One more bilinear scaling? Y/N\n");
            char c2;
            scanf(" %c",&c2);
            if(c2=='Y'||c2=='y') scaling_bi_again();
            printf("Bilinear scaling done!\n");
            break;
        default:
            printf("Wrong number. Program exit.\n");
            break;
    }
}
void make_LPF(void){
    filter = (double*)malloc(sizeof(double)*FILTER_SIZE);
    *(filter) = -0.0087;
    *(filter+1) = 0.0000;
    *(filter+2) = 0.2518;
    *(filter+3) = 0.5138;
    for(int i=4;i<FILTER_SIZE;i++) *(filter+i) = *(filter+(6-i));
    
}
void LPF_x(int length_x, int length_y, unsigned char* o_img){
    free(LPF_img);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * length_x * length_y);
    int margin = FILTER_SIZE/2;
    for (int y = 0; y < length_y; y++) {
        for (int x = 0; x < length_x; x++) {
            double sum = 0;
            for (int j = -margin; j <= margin; j++) {
                int new_x = x + j;
                if (new_x >= 0 && new_x < length_x) {
                    sum += (double)*(o_img+ y * length_x + new_x) * *(filter+j + margin);
                }
            }
            *(LPF_img+y * length_x + x) = (unsigned char)sum;
        }
    }
    printf("LPF_x DONE.\n");
}
void LPF_y(int length_x, int length_y, unsigned char* sc_x){
    free(LPF_img);
    LPF_img = (unsigned char*)malloc(sizeof(unsigned char) * length_x * length_y);
    int margin = FILTER_SIZE/2;

    for (int y = 0; y < length_y; y++) {
        for (int x = 0; x < length_x; x++) {
            double sum = 0.0;

            for (int i = -margin; i <= margin; i++) {
                int new_y = y + i;

                if (new_y >= 0 && new_y < length_y) {
                    sum += (double)*(sc_x+new_y * length_x + x) * *(filter+i + margin);
                }
            }
            *(LPF_img+y * length_x + x) = (unsigned char)sum;
        }
    }
    printf("LPF_y done\n");
}

// -0.0087    0.0000    0.2518    0.5138    0.2518    0.0000   -0.0087