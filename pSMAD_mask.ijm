
run("Clear Results");
setBatchMode(true);

input = getDirectory("Choose image Directory... ");

output = getDirectory("Choose saving directory...");

list = getFileList(input);

for (i = 0; i < list.length; i++)
{	
		file = input + list[i];
		open(file);
		T = getTitle();
		selectWindow(T);
		run("Split Channels");

		run("Merge Channels...", "c1=C1-"+T+" c2=C2-"+T+" c3=C3-"+T+" create");
		selectWindow(T);
		run("Stack to RGB");
		selectWindow(T+" (RGB)");
		saveAs("Jpeg",output+T+"RGB.jpg");
		selectWindow(T);
		run("Split Channels");

		selectWindow("C3-"+T);
		saveAs("Jpeg",output+T+"RED.jpg");
		selectWindow("C1-"+T);
		
		setThreshold(1000,65505);
		setOption("BlackBackground",true);
		run("Convert to Mask");	
	
		run("Set Measurements...","mean bounding redirect=C2-" + T + " decimal=4");
		run("Analyze Particles...", "size=50-500 display clear");
		saveAs("Results",output+"num_data_"+T+".csv");
		selectWindow("C2-"+T);
		saveAs("Jpeg",output+T+"GREEN.jpg");

		close();
		close();
		close(); 

		file = input + list[i];
		open(file);
		T = getTitle();
		selectWindow(T);
		run("Split Channels");
		selectWindow("C3-"+T); 
		close(); 

			
		run("Colocalization Threshold", "channel_1=C1-"+T+" channel_2=C2-"+T+" use=None channel=[Red : Green] show include");
		selectWindow("Colocalized Pixel Map RGB Image");
		run("Split Channels");
		selectWindow("Colocalized Pixel Map RGB Image (red)");
		close();
		selectWindow("Colocalized Pixel Map RGB Image (green)");
		close();
		selectWindow("Colocalized Pixel Map RGB Image (blue)");
		//run("16-bit");
		saveAs("Tiff",output+T+"MASK"+i+".tiff");	
		close();	
		
}

exit;
