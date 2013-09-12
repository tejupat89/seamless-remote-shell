##Seamless Remote Shell
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

***

###Installation and Setup
 Download zip folder or clone the repo : `git clone https://github.com/tejupat89/seamless-remote-shell.git`  
    Lets consider an example where we mount 2 remote servers on our local machine and then perform an operation that 
    involves files from both machines.      
    
 **Establish passwordless ssh login between the hosts involved in the operation.**       
 Lets say the 2 remote hosts are user1@server1(host1) and user2@server2(host2).
 It is required that passwordless login is setup between the local machine and the remote hosts and also between the
 remote hosts themselves. Lets consider an example of establishing passwordless login between local machine and host1:    
        1. Create public and private keys on local host using `ssh-keygen`     
        2. copy your local host's public key to the remote host using `ssh-copy-id -i ~/.ssh/id_rsa.pub host1`.        
           ssh-copy-id copies the local-host’s public key to the remote-host’s authorized_keys file. ssh-copy-id also 
           assigns proper permission to the remote-host’s home, ~/.ssh, and ~/.ssh/authorized_keys.    
        3. Try logging into host1 via ssh and it shouldn't require you to enter a password now : `ssh host1`    
        4. To establish passwordless login from1 host1 to host2, ssh into host1 and repeat the first 3 steps for 
           copying public keys to host2.    
        5. To establish passwordless login from1 host2 to host1, ssh into host2 and repeat the first 3 steps for 
           copying public keys to host1.     
           
 **Mounting the remote hosts via sshfs**     
 Now that passwordless ssh login is established between the hosts, mount the hosts on local machine via sshfs:     
    1. To mount host1 on directory dir1 on local machine: `sshfs host1: dir1`   
    2. To mount host2 on directory dir2 on local machine: `sshfs host2: dir2`      

 **Start the shell**      
    1. Go to the src folder inside the downloaded repository on your machine.    
    2. run `make all`    
    3. You need to have flex(a scanner generator) and bison (a parser generator) installed on the local machine. They can
    be installed by running `sudo apt-get install flex` and `sudo apt-get install bison`.     
    4. Start the shell with `./shell`. Enter commands at the prompt. The shell doesn't recognize the home sign (~). So you will have to specify the 
    complete filepath for each file you use in the operation. e.g consider the following command :     
    `cat /home/dir1/remote_file1 /home/dir2/remote_file2 | wc`     
    
<script type="text/javascript">

  var _gaq = _gaq || [];
  _gaq.push(['_setAccount', 'UA-43959808-1']);
  _gaq.push(['_trackPageview']);

  (function() {
    var ga = document.createElement('script'); ga.type = 'text/javascript'; ga.async = true;
    ga.src = ('https:' == document.location.protocol ? 'https://ssl' : 'http://www') + '.google-analytics.com/ga.js';
    var s = document.getElementsByTagName('script')[0]; s.parentNode.insertBefore(ga, s);
  })();

</script>
    
