import java.io.*;

/**
 * Process Guardian.java Executes a given process and checks whether
 * is produces some output every 30 seconds.
 * is called by java ProcessGuardian ....... command line.
 */

public class ProcessGuardian implements Runnable {


    private Process watchedProcess;
    private Thread wakeupThread;
    private InputStreamReader errorReader;
    private InputStreamReader inputReader;

    public ProcessGuardian(Thread inWakeupThread, Process inWatchedProcess){

	watchedProcess = inWatchedProcess;
	wakeupThread = inWakeupThread;
    }
    
    

    public void run(){

	try {
	    
	    do {
		
		System.err.println("Sleeping ....");
		Thread.sleep (10 * 1000);
		System.err.println("Checking streams");
		System.err.println("ErrorStream " 
				   + watchedProcess.getErrorStream().available());
		System.err.println("OutputStream " 
				   + watchedProcess.getInputStream().available());
		if (watchedProcess.getErrorStream().available() == 0 
		    && watchedProcess.getInputStream().available() == 0){
		    
		    System.err.println("Interrupting main thread");
		    wakeupThread.interrupt();
		}
		else {
		    
		    readEmpty();
		}
		
	    } while (true);
	}
	catch(InterruptedException e){

	    System.err.println("ProcessGuardian We are interrupted ");
	}
	catch(Exception e){
	    
	    e.printStackTrace();
	}
     
	
    }

    public void readEmpty(){

	
	readStreamEmpty(watchedProcess.getErrorStream(), System.err);
	readStreamEmpty(watchedProcess.getInputStream(), System.out);
    }

    public void readStreamEmpty(InputStream reader, PrintStream writer){

	byte buffer[] = new byte[1000];
	int charRead;
	try {
	    System.err.println("Checking availability");
	    if (reader.available() > 0){
		System.err.println("Checking : stream");
		charRead = reader.read(buffer);
		if (charRead > 0){
		    
		    System.out.println("Charread is " + charRead);
		    writer.write(buffer, 0, charRead -1);
		}
	    }
		
	} catch(IOException e){

	    e.printStackTrace();
	}
    }		    

	    
    public static void main(String argv[]){

	//execute command
	if (argv.length == 0){

	    System.out.println("Specify an argument please");
	}
	else {
	   
	    try {
		Process process =  Runtime.getRuntime().exec(argv);

		ProcessGuardian guardian = new ProcessGuardian(Thread.currentThread(), process);
		Thread guardianThread = new Thread(guardian);
		
		guardianThread.start();		
		try{	
		    System.out.println("Waiting for end of process ");
		    process.waitFor();
		    guardian.readEmpty();
		    
		} catch (InterruptedException e){
		    
		    System.err.println("The process doesn't respond. Kill it!");
		    process.destroy();
		}
		guardianThread.interrupt();
	    } catch(IOException e){

		e.printStackTrace();
	    }
	    
	}
    }
}
