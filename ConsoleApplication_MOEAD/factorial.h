#ifndef FACTORIAL_H_INCLUDED
#define FACTORIAL_H_INCLUDED



#endif // FACTORIAL_H_INCLUDED

int factorial(unsigned int x){

    int val_=1;
    if(x<1) cout<<"smaller than 1"<<endl;
    else if(x==1) return val_;
    else{

        for(unsigned int i=1;i<=x;++i)
            val_*=i;
    }

    return val_;
}
