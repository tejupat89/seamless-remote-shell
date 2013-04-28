Today we either log into a remote machine using SSH and perform operations there or we mount a remote directory on the 
local machine via sshfs and perform operations on the local machine. However, if you mount the remote directory and if 
the files on the remote directory are as large as 1GB, then the operations tend to be very slow. This is because while 
executing the command on BASH, it tries to download the entire remote file over the network to the local machine before 
performing operations on it.   

In this project we identify which files in the operation are remote and which ones are local. If there are remote files 
involved in the operation then the operation is performed on the remote machine by logging into it via ssh. This results 
in faster execution as the entire doesn't have to be downloaded before performing operations on it.

**Examples of what the project does**
- A simple example would be considering the following command:    

  *cat  /home/remote/1gb.img | wc*    
  In this example 1gb.img is a 1GB file wich is present on the remote machine. The remote directory on the remote server 
has been mounted on the local machine via sshfs. A command like this in BASH would take a very long time to run since BASH
would try to download the file to the local machine first. However in the project, this command would get transformed to 
the following:  

  *ssh  server  " cat   /home/1gb.img   |   wc"*
  
- A slightly complex example would be the following:

  *cat   /home/remote1/1gb.img   /home/remote2/100mb.img  |  wc*
  
  In this example, there are 2 remote servers involved in the operation. One remote directory has been mounted on remote1 
  directory and the other on remote2 directory on the local machine. Since there is more than 1 file and server involved 
  in the operation, query planning comes into play. Query planning is done based on the amount of data transfer between 
  the two servers and the local machine. Following possibilities will be considered before deciding as to where the command 
  should be executed - whether on the local machine, on remote server1 or on remote server2:   
  1. Data transfer needed to execute command on remote server 1 : 100MB (since 100mb.img is located on server2)
  2. Data transfer needed to execute command on remote server 2 : 1GB (since 1gb.img is located on server1)
  3. Data transfer needed to execute command on local machine : 1GB + 100MB (since both are located on remote servers)

  Since the first choice requires the least amount of data transfer, the operation would execute faster if it executed on 
  server 1. So the 100mb.img file on server2 is copied to server1 via scp and then the command is transformed to the 
  following before execution :

  *ssh  server1  "cat  /home/1gb.img  /home/100mb.img  |  wc"*
