out:pnlg.o
   gcc pnlg.o -o out
pnlg.o:pnlg.c
   gcc -c pnlg.c
clean:
   rm -rf *.o out
