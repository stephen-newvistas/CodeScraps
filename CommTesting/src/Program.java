import java.io.IOException;
import java.util.Arrays;
import java.util.List;



public class Program{
	public static void main( String[] args ) {

		Queue queue = new Queue();
		queue.MonitorInput();
		
//		while( true){
//
//			char c[] = new char[]{ 0xFE , 0x00 , 0x0D , 0x0A , 0x83 };
////			char c[] = new char[]{ 0xFE ,  0x7F , 0x80 , 0x7F , 0x81 , 0x7F , 0x83 , 0xFC };
//			char d[] = new char[ 2 * c.length ];
//			java.util.Arrays.fill( d , '1' );
//			int primaryIndexer = 0 , secondaryIndexer = 0;
//			for( primaryIndexer = 0 ; primaryIndexer < c.length ; primaryIndexer++ ){
//				if( c[ primaryIndexer ] == 0x00 ){
//					d[ secondaryIndexer++ ] = 0x7F;
//					d[ secondaryIndexer++ ] = 0x80;
//				}
//				else if( c[ primaryIndexer ] == 0x0D ){
//					d[ secondaryIndexer++ ] = 0x7F;
//					d[ secondaryIndexer++ ] = 0x81;
//				}
//				else if( c[ primaryIndexer ] == 0x0A ){
//					d[ secondaryIndexer++ ] = 0x7F;
//					d[ secondaryIndexer++ ] = 0x82;
//				}
//				else
//					d[ secondaryIndexer++ ] = c[ primaryIndexer ];				
//			}
//			c = d;
//			String s = new String( d );
//			s.trim();
//			
//			try {
//				queue.SendtoQueue( new String( c ) );
//			} catch (IOException e1) {
//				// TODO Auto-generated catch block
//				e1.printStackTrace();
//			}
//			
//			try {
//				Thread.sleep( 1000 );
//			} catch (InterruptedException e) {
//				// TODO Auto-generated catch block
//				e.printStackTrace();
//			}
//		}
	}
	
	public static class Auto{
		
		static Auto auto;
		
		public Auto GetThis(){
			return this;
		}
		
		public void ProcessIncomingMessage_packet( String _message ){
			
			
		}
		
	}
	
	
}


