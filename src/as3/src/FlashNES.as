package
{
	import cmodule.libNES.CLibInit;
	
	import flash.display.Bitmap;
	import flash.display.BitmapData;
	import flash.display.Sprite;
	import flash.events.Event;
	import flash.geom.Rectangle;
	import flash.net.URLLoader;
	import flash.net.URLLoaderDataFormat;
	import flash.net.URLRequest;
	import flash.utils.ByteArray;
	
	import utils.FPSCounter;
	
	[SWF(width=512,height=448,frameRate=60)]
	public class FlashNES extends Sprite
	{
		public var cNES:Object;
		public var cNESLoader:CLibInit;
		
		public var romLoader:URLLoader;
		
		public var screen:Bitmap;
		public var screenData:BitmapData;
		public var screenRectangle:Rectangle;
		
		public var cRam:ByteArray;
		
		public function FlashNES()
		{
			//http://10.0.1.4/nesTest.html?debug=true
			// Load Rom
			var romPath:String = "http://10.0.1.4/test_roms/nes/Mario3.nes";
			
			romLoader = new URLLoader;
			romLoader.addEventListener(Event.COMPLETE, onRomLoaded);
			romLoader.dataFormat = URLLoaderDataFormat.BINARY;
			romLoader.load( new URLRequest(romPath) );
			
			// Display
			screen = new Bitmap;
			screen.scaleX = 2;
			screen.scaleY = 2;
			screenData = new BitmapData( 256, 240, false, 0x0 );
			
			screen.bitmapData = screenData;
			
			addChild(screen);
			
			// Frame rate
			var fps:FPSCounter = new FPSCounter();
			addChild(fps);
			
			// Ram pointer
			var ramNS:Namespace = new Namespace("cmodule.libNES");
			cRam = ((ramNS)::gstate).ds;
			
			screenRectangle = new Rectangle( 0, 0, 256, 240 );
		}
		
		public function onRomLoaded(e:Event):void
		{
			// Initialize C-side
			cNESLoader = new CLibInit;
			
			cNESLoader.putEnv("SDL_VIDEODRIVER", "flash");
			cNESLoader.putEnv("SDL_AUDIODRIVER", "flash");
			cNESLoader.supplyFile( "nesrom", romLoader.data );
			
			cNES = cNESLoader.init();
			
			// Start emulation
			cNES.setup();
			
			// Tick
			addEventListener( Event.ENTER_FRAME, onEnterFrame );
		}
		
		public function onEnterFrame(e:Event):void
		{
//			trace("\n\nloop\n\n\n");
			cNES.loop();
			
//			trace(cNES.screen().toString());
			// Update screen
			cRam.position = cNES.screen() + 8 * 256 * 4;	// Advance pointer forward 8 rows -- those are blank
			screenData.setPixels( screenRectangle, cRam );
		}
	}
}