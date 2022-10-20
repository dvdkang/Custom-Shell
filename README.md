# Custom-Shell Tests

Testing ls and pwd
![](example/Screen%20Shot%202022-10-07%20at%2012.48.02%20AM.png)

Here we can see that our code properly executes the following inputs ls and pwd when in our custom shell.

Testing cd
![](example/Screen%20Shot%202022-10-07%20at%2012.53.52%20AM.png)

FIrst verifying the location of our shell using ls, after we call cd and ls again, we notice that we are not put into the parent folder of the previous files we were accessing. 

Tesitng ';'
![](example/Screen%20Shot%202022-10-07%20at%2012.52.13%20AM.png)

We see that we run one after the other without having to call them one at a time. 

Testing '&'
![](example/Screen%20Shot%202022-10-07%20at%2012.52.26%20AM.png)

Our sleeps are caught by the shell and run sleep 1 together and then sleep 1 after those 2 have ran. They give us a total of 2 seconds of sleep. 
