#include <iostream>
#include <string>
#include <thread>
#include <fstream>
using namespace std;
struct cell;
struct index;
cell** init(int (*board)[9],cell **cells, bool &failed);
index get_square(index inx);
index get_square_hyper(index inx);
void find_true_list(int *vals ,cell c);
int find_true(cell c);
index find_min_possible(cell **cells);
bool reduce_possib(index in, cell** cells, int value, cell **newcells);
void cell_to_board(int (*board)[9], cell **cells);
void foo(int (*b)[9]);
bool valid_sudoku(cell** cells);
bool check_row(int i ,cell **cells);
bool check_col(int j, cell **cells);
bool check_squre(index sqin, cell **cells);
bool sudoku_finished(cell **cells);
void print_state(cell **c);

void read_file(int (*board)[9],string str)
{
   
   
    ifstream fp(str);
for(int row = 0; row < 9; row++) {  // stop loops if nothing to read
   for(int column = 0; column < 9; column++){
        fp >> board[row][column];
       
    }
    
}
}


struct cell{
    bool possibs[10]={false,true,true,true,true,true,true,true,true,true};
    int count=9;
};

struct index{
    int i;
    int j;
};
void mcheck_row(int i,cell** cells,bool *flag)
{
    *flag=check_row(i,cells);
}
void mcheck_col(int j, cell** cells, bool *flag)
{
    *flag=check_col(j,cells);
}

void mcheck_squre(index in ,cell** cells, bool *flag)
{
    *flag = true;
}

bool multi_thread_valid(cell** cells)
{
    //check for all rows and cols that they are unique
    bool row_flags[9];
     bool col_flags[9];
     bool msqr_flags[9];
     bool hsqr_flags[4];
     thread row_threads[9];
     thread col_threads[9];
     thread msqr_threads[9];
     thread hsqr_threads[4];
    
     for (int i = 0; i < 9; i++)
     {
        row_threads[i]= thread(mcheck_row,i,cells,&row_flags[i]);
         col_threads[i] = thread(mcheck_col,i,cells,&col_flags[i]);
    }
     //check for all main squares
     int ct=0;
    for (int i = 0 ; i < 9; i+=3)
    {
        for (int j = 0; j < 9; j+=3)
        {
            index inx={i,j};
            msqr_threads[ct] =thread(mcheck_squre,inx,cells,&msqr_flags[ct]);
                ct++;
        }
        
        
     }
     ct=0;
    //check for hyper squares
    for (int i = 1; i < 9; i+=4)
    {
        for (int j = 1; j < 9; j+=4)
        {
            index inx={i,j};
            hsqr_threads[ct] =thread(mcheck_squre,inx,cells,&hsqr_flags[ct]);
                ct++;
        }
        
    }
    
    for(int i =0; i<9 ; i++)
    {
         row_threads[i].join();
         if(!row_flags[i])
            return false;
         col_threads[i].join();
         if(!col_flags[i])
             return false;
         msqr_threads[i].join();
         if(!msqr_flags[i])
             return false;
    }
    for (int j = 0; j < 4; j++)
    {
        hsqr_threads[j].join();
        if(!hsqr_flags[j])
            return false;
    }
    
    return true;
}

//bool solve=false;
//cell **gcells;

void multi_thread(cell **gcells, bool solve)
{
    
    if(sudoku_finished((gcells)))
        {
            if(!solve)
            {

                int b[9][9];
                cell_to_board(b,gcells);
                foo(b);
                print_state(gcells);
            }
            solve=true;
            return;

        }
    index in = find_min_possible(gcells);
    int ct =gcells[in.i][in.j].count;
    int vals[ct];
    find_true_list(vals,gcells[in.i][in.j]);
    thread tlist[ct];
    //newcells
   
    
    for (int i = 0; i < ct; i++)
    {   
         cell **newcells= new cell*[9];
    for(int i = 0 ; i < 9;i++)
        newcells[i]=new cell[9];
        reduce_possib(in,gcells,vals[i],newcells);
             
        if(valid_sudoku(newcells))
        {
            tlist[i]=thread(multi_thread,newcells,ref(solve));
        
        }
        
    }
    for (int i = 0; i < ct; i++)
    {
        
            tlist[i].join();
    }
    
   
        
    
    
    
}



cell** backtrack(cell** cells,bool &solve )
{
    if(sudoku_finished(cells))
    {
        solve =true;
        return cells;
    }
    index in = find_min_possible(cells);
    int ct= cells[in.i][in.j].count;
    int vals[ct];
    find_true_list(vals,cells[in.i][in.j]);
    //newcells
    cell** newcells= new cell*[9];
         for(int i = 0 ; i < 9;i++)
             newcells[i]=new cell[9];

    for (int i = 0; i < ct; i++)
    {
        reduce_possib(in,cells,vals[i],newcells);
            // int b[9][9];
            // cell_to_board(b,newcells);
            // foo(b);
            // print_state(newcells);
        if(valid_sudoku(newcells))
        {
            cell** temp= backtrack(newcells,solve);
            if(solve==true)
                return temp;
        }
    }
     for(int i = 0 ; i < 9;i++)
            delete [] newcells[i];
     return cells;


}
bool sudoku_finished(cell **cells)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if(cells[i][j].count !=1)
                return false;
        }
        
    }
    return true;
}

bool check_row(int i ,cell **cells)
{
    int num[10]={0,0,0,0,0,0,0,0,0,0};
    for (int c = 0; c < 9; c++)
    {
        if(cells[i][c].count==1)
        {
            int val=find_true(cells[i][c]);
            num[val]++;
            if(num[val]>1)
            return false;
        }
        

    }
    return true;

}
bool check_col(int j, cell **cells)
{
    int num[10]={0,0,0,0,0,0,0,0,0,0,};
    for (int c = 0; c < 9; c++)
    {
        if(cells[c][j].count==1)
        {
             int val=find_true(cells[c][j]);
            num[val]++;
            if(num[val]>1)
            return false;
        }
    }
    return true;
}
bool check_squre(index sqin, cell **cells)
{
    int num[10]={0,0,0,0,0,0,0,0,0,0,};
    for (int i = sqin.i; i < 3+sqin.i; i++)
    {
        for (int j = sqin.j; j < 3+sqin.j; j++)
        {
            if(cells[i][j].count==1)
            {
                int val=find_true(cells[i][j]);
                num[val]++;
                if(num[val]>1)
                    return false;
            }
        }
        
    }
    return true;
    
}

void cell_to_board(int (*board)[9], cell **cells)
{
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if(cells[i][j].count==1)
            {
                board[i][j]=find_true(cells[i][j]);
            }
            else
            {
                board[i][j]=0;
            }
            
        }
        
    }
    
}
bool valid_sudoku(cell** cells)
{
    //check for all rows and cols that they are unique
    for (int i = 0; i < 9; i++)
    {
        if(!check_row(i,cells))
            return false;
        if(!check_col(i,cells))
            return false;
    }
    //check for all main squares
    for (int i = 0; i < 9; i+=3)
    {
        for (int j = 0; j < 9; j+=3)
        {
            index in ={i,j};
            if(!check_squre(in ,cells))
                return false;
        }
        
    }
    //check for hyper squares
    for (int i = 1; i < 9; i+=4)
    {
        for (int j = 1; j < 9; j+=4)
        {
            index in = {i, j};
            if(!check_squre(in, cells))
                return false;
        }
        
    }
    
    return true;
    
    
}

bool reduce_possib(index in, cell** cells,int value ,cell** newcells)
{
        //check if the value is in possiblities of cell or not
        if(!cells[in.i][in.j].possibs[value])
            return false;

            //copy to newcells
        for (int i = 0; i < 9; i++)
        {
            for (int j=0;  j < 9;  j++)
            {
                newcells[i][j]=cells[i][j];
            }
            
        }
        //set value for cell in
        for (int i = 1; i < 10; i++)
            newcells[in.i][in.j].possibs[i]=false;
        newcells[in.i][in.j].possibs[value]=true;
        newcells[in.i][in.j].count = 1;

        //reduce row
        for (int j = 0; j < 9; j++)
        {
            if(newcells[in.i][j].possibs[value]!=false && j!=in.j)
            {
                newcells[in.i][j].possibs[value]= false;
                newcells[in.i][j].count --;
            } 
        }
        //reduce col
        for (int i = 0; i < 9; i++)
        {
            if(newcells[i][in.j].possibs[value]!=false && i!=in.i)
            {
                newcells[i][in.j].possibs[value]= false;
                newcells[i][in.j].count --;
            } 
        }

        //reduce squares
        index sqrin = get_square(in);
    index sqrin_hyp = get_square_hyper(in);
    //reduce main square
    for(int i=sqrin.i; i<3+ sqrin.i;i++)
    {
        for(int j=sqrin.j;j<3+sqrin.j;j++)
        {
            if (newcells[i ][j].possibs[value]!=false && i!=in.i && j!=in.j)
            {
               newcells[i][j].possibs[value]= false;
                newcells[i][j].count --;
            }
        }
    }

    //reduce hyper square
        if(sqrin_hyp.i!=-1 && sqrin_hyp.j!=-1)
        {
            for(int i=sqrin_hyp.i; i<3+sqrin_hyp.i;i++)
        {
        for(int j=sqrin_hyp.j;j<3+sqrin_hyp.j;j++)
        {
            if (newcells[i ][j].possibs[value]!=false && i!=in.i && j!=in.j)
            {
               newcells[i][j].possibs[value]= false;
                newcells[i][j].count --;
            }
        }
        }
        }

       return true; 

}


index find_min_possible(cell **cells)
{
    int min =9;
    index minin;
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if(cells[i][j].count!=1)
            {
                if(cells[i][j].count < min)
                {
                    min = cells[i][j].count;
                    minin.i=i;
                    minin.j=j;
                }
            }
        }
        
    }
    return minin;
    

}

void foo(int (*b)[9])
{
    for(int i=0;i<9;i++)
    {
    for(int j=0;j<9; j++)
    {
        cout<<b[i][j]<<" " ;
    }
    cout<<endl;
    }
}

void print_state(cell **c)
{
    for(int i=0;i<9;i++)
    {
        for(int j=0; j<9; j++)
        {
            int ct = c[i][j].count;
                int *vals= new int[ct];
                find_true_list(vals,c[i][j]);
            for(int k=0; k< ct;k++)
            {
                cout<< vals[k]<< ",";
            }
            delete [] vals;
            cout<<" | ";
        }
        cout<<endl;
    }
}
int find_true(cell c)
{
    for(int i=1; i<=9; i++)
       {

       if(c.possibs[i]==true)
            return i;
       } 
            return -1;
        
}
void find_true_list(int *vals ,cell c)
{
    int ct =0;
    for(int i=1; i<=9 ; i++)
    {
        if(c.possibs[i]==true)
        {
            vals[ct++] = i;
        }
        if(ct==c.count)
        break;
    }
}

cell** init(int (*board)[9],cell **cells, bool &failed)
{
   
    for(int i=0;i<9;i++)
    {

        for(int j=0; j<9; j++)
        {
            
            if(board[i][j]!=0)
            {
              
                index inx;
            inx.i =i;
            inx.j=j;
            cell** newcells= new cell*[9];
            for(int i = 0 ; i < 9;i++)
                newcells[i]=new cell[9]; 
            if(!reduce_possib(inx, cells,board[i][j],newcells))
                {
                    failed = true;
                    break;
                }
            for(int i = 0 ; i < 9;i++)
                delete [] cells[i];
            cells =newcells; 
            }
           
         
        }
    }
    return cells;
}

index get_square(index inx)
{
    //return the index of main squre that cell is in it 
    index res;
    res.i= 3*(inx.i/3);
    res.j = 3*(inx.j/3);
    return res;
}
index get_square_hyper(index inx){
    //return the the index of hyper squre that cell is in it and -1 for not find
    index res;
    if(inx.i<=3 && inx.i>=1)
        res.i = 1;
    else if(inx.i>=5 && inx.i<=7)
        res.i = 5;
    else
        res.i=-1;

    if(inx.j<=3 && inx.j>=1)
        res.j = 1;
    else if(inx.j>=5 && inx.j<=7)
        res.j = 5;
    else
        res.j=-1;
    return res;
}


int main()
{
    // int board[9][9]={
    //     0,0,0,0,0,0,0,2,0,
    //     0,0,1,0,5,2,7,0,0,
    //     0,0,0,0,0,4,0,0,0,
    //     0,0,3,0,6,0,0,0,7,
    //     0,0,0,4,9,0,0,1,0,
    //     0,0,0,0,0,0,0,0,8,
    //     0,0,2,6,0,3,9,8,0,
    //     0,0,0,0,0,0,0,0,0,
    //     0,0,0,5,1,9,0,0,0};
    int board[9][9];
    read_file(board, "sudoku.txt");
        cell** mycells= new cell*[9];
        for(int i = 0 ; i < 9;i++)
        mycells[i]=new cell[9];

bool failed=false;
mycells=init(board, mycells, failed);
if(!failed){
//check multi_thread_valid
//cout << multi_thread_valid(mycells)<<endl;
bool solve =false;
mycells = backtrack(mycells,solve);
cell_to_board(board,mycells);
foo(board);
}
else{
    cout <<"invalid sudoku";
}
    return 0;
}


