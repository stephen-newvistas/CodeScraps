import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.concurrent.*;


public class Queue{
	
	//Auto auto;
	
	public Queue(){
		MonitorInput(); 
	}
	
	static ExecutorService watchExecutor = Executors.newSingleThreadExecutor();	
	static ExecutorService sendExecutor = Executors.newSingleThreadExecutor();
	static ExecutorService getExecutor = Executors.newSingleThreadExecutor();
	
	public void MonitorInput( ){ 
		watchExecutor.submit( new Runnable() {
			@Override
		public void run(){
				try {
					while( true )
						GetFromQueue();
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		});
	}
	
	static void ProcessInput( final String _input ){ //final Auto _auto , final String _input
		getExecutor.submit( new Runnable() {
			@Override
			public void run(){
				// Auto auto = Auto.GetThis();
				// auto.process.ProcessIncomingMessage_packet( _input );
				GetPacket( _input );
				System.out.println( "Message From Queue: " + _input );
			}
		});
	}
	
	private static void GetFromQueue() throws IOException{
		Runtime rt = Runtime.getRuntime();
		String[] command = { "queue" , "get" , "7" };
		Process process = rt.exec( command );
		
		BufferedReader stdInput = new BufferedReader( new InputStreamReader( process.getInputStream() ) );
		BufferedReader stdError = new BufferedReader( new InputStreamReader( process.getErrorStream() ) );

		String s;
		while( true ){
			if( (s = stdInput.readLine() ) == null )
				return;
			Queue.ProcessInput( s );
			rt.exec( command );
		}
	}
	
	
	public void SendOutput( final String _output ){
		sendExecutor.submit( new Runnable() {
			@Override
			public void run(){
				try {
					SendtoQueue( _output );
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		});
	}

	static void SendtoQueue( String _message ) throws IOException{
		Runtime rt = Runtime.getRuntime();
//		String[] command = { "queue" , "send" , "6" , EscapeCharacters( _message )  };
		String[] command = { "queue" , "send" , "1" , _message  };
		Process process = rt.exec( command );
	}
	
	public static void exit(){
		sendExecutor.shutdownNow();
		getExecutor.shutdownNow();
		watchExecutor.shutdownNow();
	}
	

	public static void GetPacket( String _message ){
//		Packet packet = new Packet();
		byte[] message = _message.getBytes();
		byte[] packetArray = new byte[ _message.length() / 2 ];
		int primaryIndexer = 0 , secondaryIndexer = 0;
		for( primaryIndexer = 0 ; primaryIndexer < packetArray.length ; secondaryIndexer++ ){
			packetArray[ secondaryIndexer ] = (byte) ( (ConvertFromAscii( message[ primaryIndexer++ ]) << 4 ) | (ConvertFromAscii( message[ primaryIndexer++ ])) );
			
			
//			packetArray[ secondaryIndexer ] = (byte) ( ( Byte.valueOf( message[ primaryIndexer++ ] ) << 4 ) | ( Byte.valueOf( message[ primaryIndexer++ ] ) ) );
			
		}
		
//		this.auto.process.IncomingMessage_Packet( packet );
	}
	
	public static byte ConvertFromAscii( byte _ascii ){
		switch( _ascii ){
			case 48:
				return 0;
			case 49:
				return 1;
			case 50:
				return 2;
			case 51:
				return 3;
			case 52:
				return 4;
			case 53:
				return 5;
			case 54:
				return 6;
			case 55:
				return 7;
			case 56:
				return 8;
			case 59:
				return 9;
			case 65:
				return 10;
			case 66:
				return 11;
			case 67:
				return 12;
			case 68:
				return 13;
			case 69:
				return 14;
			case 70:
				return 15;
			default:
				return 0;
		}		
	}
	
	private static String EscapeCharacters( String _message ){
		String message = _message;
		
		//	create sequences for escaped characters
		//	null character
		String zero = Integer.toString( 0 );
		String newZero = new String( Integer.toString( 127 ) + Integer.toString( 128 ) );
		//	carriage return character
		String cr = Integer.toString( 13 );
		String newCR = new String( Integer.toString( 127 ) + Integer.toString( 129 ) );
		//	line feed character
		String lf = Integer.toString( 10 );
		String newLF = new String( Integer.toString( 127 ) + Integer.toString( 130 ) );
		
		//	replace all escaped characters with sequence
		message.replaceAll( zero , newZero );
		message.replaceAll( cr , newCR );
		message.replaceAll( lf , newLF );			
		
		return message;
	}
	

	public static byte[] toUnescapedByteArray( byte[] _escaped ){
		byte[] a = new byte[ _escaped.length ];
		int primaryIndexer = 0 , secondaryIndexer = 0;
		for( primaryIndexer = 0 ; primaryIndexer < _escaped.length ; primaryIndexer++ ){
			if( _escaped[ primaryIndexer ] == 127 ){
				primaryIndexer++;
				if( _escaped[ primaryIndexer ] == 0x80 )
					a[ secondaryIndexer++ ] = 0x00;
				else if( _escaped[ primaryIndexer ] == 0x81 )
					a[ secondaryIndexer++ ] = 0x01;
				else if( _escaped[ primaryIndexer ] == 0x82 )
					a[ secondaryIndexer++ ] = 0x02;
			}
			else
				a[ secondaryIndexer++ ] = a[ primaryIndexer ];				
		}
		
		return a;
	}
	
}


