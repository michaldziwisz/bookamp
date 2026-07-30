#define main enhance_main_unused
#include "../src/enhance.cpp"
#undef main
#include <cstdio>
#include <cstring>
#include <vector>
#include <string>
#include <cmath>
using namespace bookamp;

int main(){
    FILE* f=fopen("/tmp/tr_ring24.csv","r");
    if(!f){printf("no csv\n");return 2;}
    char line[4096];
    fgets(line,sizeof line,f); // header
    int bad=0,tot=0; int perw[12]={0};
    while(fgets(line,sizeof line,f)){
        // cols: i,in,c6,c7,c4,o68,r0..r11
        std::vector<std::string> col; char* p=strtok(line,",");
        while(p){col.push_back(p);p=strtok(nullptr,",\n");}
        if(col.size()<18) continue;
        int c6=atoi(col[2].c_str());
        int wpos=((c6-1)%12+12)%12;
        uint32_t gtu; float gt=strtof(col[5].c_str(),nullptr); memcpy(&gtu,&gt,4);
        LMCore lm; memset(&lm,0,sizeof lm);
        for(int k=0;k<12;k++) lm.ispHist[k]=strtof(col[6+k].c_str(),nullptr);
        lm.ispPos=wpos;
        // detect writes ispHist[wpos]=xi; the csv r[] is buffer AFTER write, so xi already stored.
        float xi=lm.ispHist[wpos];
        float got=lm.detect(xi);
        uint32_t gu; memcpy(&gu,&got,4);
        long du=labs((long)gu-(long)gtu);
        tot++;
        if(du!=0){ bad++; perw[wpos]++;
            if(bad<=15) printf("wpos=%d gt=%.9g got=%.9g ULP=%ld\n",wpos,gt,got,du);
        }
    }
    fclose(f);
    printf("TOT=%d BAD=%d  per-wpos:",tot,bad);
    for(int w=0;w<12;w++) if(perw[w]) printf(" %d:%d",w,perw[w]);
    printf("\n%s\n", bad==0?"DETECT_BITEXACT_OK":"DETECT_FAIL");
    return bad?1:0;
}
