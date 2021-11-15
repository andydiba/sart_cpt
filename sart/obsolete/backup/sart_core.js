
/** Overwrite these in html file header to make test SART/CPT, DEMO or real and adjust symbols*/

var TYPE = "sart";   /*cpt*/

var DEMO = true;     

var MYSYMBOLS = NUMBERS; //or FRUITS etc;

//var OMIT = new Array(ORANGE, LEMON);   //OMIT for sart, ACCEPT for CPT

var OMIT = new Array("2","6");

//var MYSYMBOLS = NUMBERS;

//var OMIT = new Array("3", "5");   //OMIT for sart, ACCEPT for CPT



//RUNS, REPS    for demo set RUNS to 10000, REPS 2

var goTo = null;

var LOCKED=false;

function loadStates(nruns){
	
	var dest=[];
	var i;
	
	for(i =0; i<nruns/2;i++)
	   dest.push("ctl");
	   
	for(i =nruns/2; i<nruns;i++)
	   dest.push("exp");
	
	return dest;
}


function loadSymbols(symbolFamily){
	
	var dest=[];
	var L = symbolFamily.length;
	
	for(var j=0;j<REPS;j++)
		for(var i =0; i<L;i++)
			dest.push(symbolFamily[i]);
			
	return dest;	
}


function resetScores(){
	
	FP=0;
	FN=0;
	TP=0;
	TN=0;
	
	document.getElementById("fp").innerHTML = FP;
	document.getElementById("tp").innerHTML = TP;
	document.getElementById("fn").innerHTML = FN;
	document.getElementById("tn").innerHTML = TN;
}



function logRT(){
	if(RT0!=0){
		
		var obj ={};
		obj.sym = sym;
		obj.react_ms = RT; //RT=0 => no repsonse
		
		result.push(obj);	
		
		document.getElementById("rt").innerHTML = RT;
	}	
}



function setMask(){
	
	displayBx.className = "f4";
	displayBx.style.left = 0;
	displayBx.style.top = 0;
	
	//remove image !
	clearImage(displayBx);
	
	displayBx.innerHTML = "";
	displayBx.innerHTML = MASK;
	
	hTimer = setTimeout(setRandSymbol,MASK_DURATION);
		
}



function countDown(n){

	if(n==0){
		setRandSymbol();  //begins the test
		
		if(!DEMO)
		{
			if(state[run]=="exp")   //start array if applicable
				ajax_rq("array_on.txt");  
		}	
		
	
	}else {
		displayBx.className = "f4";
		
		msg ="";
		for(var i=0;i<n;i++)
			msg+=".";

		hTimer = setTimeout(function(){countDown(n-1);},1000);
		displayBx.innerHTML=msg;
	}

}




function checkPrevious(s){
	
	
	if(TYPE=="sart"){
		//SART config
		if(CLICKED!=-1)
		{
			if( OMIT.includes(s)) 
			{
				if(CLICKED==1){ 
					FP++;
					document.getElementById("fp").innerHTML = FP;
				}else{ //clicked ==0 
					TN++;
					document.getElementById("tn").innerHTML = TN;
				}
				
			}else{
				
				if(CLICKED==1){
					TP++;
					document.getElementById("tp").innerHTML = TP;
				}else{ //clicked ==0
					FN++;
					document.getElementById("fn").innerHTML = FN;
				}
				
			}
		}
	}else{ //cpt
		//CPT config
		if(CLICKED!=-1)
		{
			if( OMIT.includes(s)) 
			{
				if(CLICKED==1){ 
					TP++;
					document.getElementById("tp").innerHTML = TP;
				}else{ //clicked ==0 
					FN++;
					document.getElementById("fn").innerHTML = FN;
				}
				
			}else{
				
				if(CLICKED==1){
					FP++;
					document.getElementById("fp").innerHTML = FP;
				}else{ //clicked ==0
					TN++;
					document.getElementById("tn").innerHTML = TN;
				}
				
			}
		
		}	
	}
		
	CLICKED=0;
	
}



function feedback(s){
	
	
	if( OMIT.includes(s) ) 
	{
		if(TYPE=="sart")
			displayBx.style.color = "red";
		else  //CPT
			displayBx.style.color = "green";
	
	}else{//all other symbols 
		
		if(TYPE=="sart")
			displayBx.style.color = "green";
		else  //CPT
			displayBx.style.color = "red";
	}
}




function reset(){
	
	
	clearAlert();
	
	/*clear result, reload and randomize symbols, reset timer/flags etc.*/
	result =[];
	
	
	symbols = loadSymbols(MYSYMBOLS);
	
	count = symbols.length;     
	//halfway
	halfway=Math.round(count/2); halfwayflg=0;
	
	symbols = shuffle(shuffle(shuffle(symbols)));
	

	KEY_EN = false;
	hTimer = null;
	RT0=0;
	
	document.body.style.backgroundColor = DEFAULT_BKG; 
	displayBx.className = "f4";
	displayBx.style.left = 0;
	displayBx.style.top = 0;
	displayBx.style.color = DEFAULT_CLR;
	displayBx.innerHTML = "";
	
	CLICKED = -1;
	
	resetScores();
}



function sart_quit(){
	
	clearTimeout(hTimer);
	hTimer = null;
	KEY_EN = false;
	
	if(!DEMO)
	{
		results.pop();
	
		if(state[run]=="exp")
			ajax_rq("array_off.txt");  //stop
	}
	
	
	document.body.style.backgroundColor = "red"; 
	alert("Test Aborted!\n Hit [SPACE] to restart");
		
}
function clearImage(elem){
	
	elem.style.backgroundImage = "none";
}

function preloadImages(){
	console.log('Preloading images');
	for(var i=0;i<10;i++)
	{
		var img = new Image();
		
		img.src = 'image'+i+'.png';
	}
}

function setImageOverride(sym,elem){
	displayBx.innerHTML = "";  //clear the mask symbol
	//based on symbol, overrride with an image
	switch(sym){
		
		case "0":
		elem.style.backgroundImage = "url('image0.png')";
		break;
		
		case "1":
		elem.style.backgroundImage = "url('image1.png')";
		break;
		
		case "2":
		elem.style.backgroundImage = "url('image2.png')";
		break;
		
		case "3":
		elem.style.backgroundImage = "url('image3.png')";
		break;
		
		case "4":
		elem.style.backgroundImage = "url('image4.png')";
		break;
		
		case "5":
		elem.style.backgroundImage = "url('image5.png')";
		break;
		
		case "6":
		elem.style.backgroundImage = "url('image6.png')";
		break;
		
		case "7":
		elem.style.backgroundImage = "url('image7.png')";
		break;
		
		case "8":
		elem.style.backgroundImage = "url('image8.png')";
		break;
		
		case "9":
		elem.style.backgroundImage = "url('image9.png')";
		break;
		
		default:
			elem.style.backgroundImage = "url('image0.png')";
	}
		
	
}

function clearBreak(){
	
	setMask();
	clearAlert();
}

function setRandSymbol(){

	if(count>0){
		
		//if count is halfway we could pause...
		
		
		
		checkPrevious(sym);
		logRT();  //logs previous [RT , sym] 
		
		
		//now would be a good time to pause...and call setMask()+clearAlert() = clearBreak to kickstart again later
		if(count == halfway && !halfwayflg && !DEMO)
		{
			KEY_EN = false;
			halfwayflg=1;
			setAlert("take a break, you're halfway there",10,clearBreak);
			return;
		}
				
		var now = new Date();
		RT0 = now.getTime();
		RT = 0;

		//pop random number from array and assign to current symbol -> 'sym'
		sym = symbols.pop();

		var rd1 =  Math.random();
		var rd2 =  Math.random();

		//Randomize Font and Position  (set 5 to 0 to override and have just 1 font)
		var font = "f"+(Math.floor(5*rd1)).toString();  //[0,4]

		var pos_x = ((rd1-0.5)*XY_SHIFT_MAX).toString()+"px";
		var pos_y = ((rd2-0.5)*XY_SHIFT_MAX).toString() +"px";
		
		displayBx.className = font;
		displayBx.style.left = pos_x;
		displayBx.style.top = pos_y;
		displayBx.style.color = DEFAULT_CLR;
		
		//displayBx.innerHTML = sym;  //could override with an image?
	   setImageOverride(sym,displayBx);

	    hTimer = setTimeout(setMask,SYM_DURATION);	
		KEY_EN = true;
		
		count--;
		
		
	}else{
		//finished this run
		clearTimeout(hTimer);
		hTimer = null;
		KEY_EN = false;
		
		checkPrevious(sym);
		logRT();  //get last RT  
		
		
		
		
		if(!DEMO)
		{
			//shut off the array (regardless of state..just ask anyways!)
				ajax_rq("array_off.txt");  //stop
		}
		
		run++;
		
		
		
		var obj = {fp:FP, tp:TP, tn:TN, fn:FN};		
		results.push(obj);	
		
		
		//log latest run in 'results'
		results.push(result);
		console.log(results);
		
		if(!DEMO)
		{	
			/*fix this with setAlerts... 'runs' only has meaning here*/
			if(run<RUNS){
				
				window.onkeypress = null; //disable user input  (optional and reset in sart_start)
				
				setAlert("Test "+run+" complete! The next round starts in<br>",
				20,
				function(){sart_start()});

			}else{	//finished this overall test
				
				window.onkeypress = null; //disable user input
				
				//Either upload via POST or download via browser (or both?)
				//Note: test type needs to be in filename to ensure unique name

				var testType = results[0].type;

				download(pid+"_"+testType+"_"+group, JSON.stringify(results));	
				upload("uploader.txt",pid+"_"+testType+"_"+group+"$"+JSON.stringify(results));
	
				
				if(goTo!=null){
					
					setAlert("Finished this test. Moving on to practice session of next type<br>",
					20,
					function(){window.location.assign(goTo+"?pid="+pid+"&group="+group)});
					
				}else{  //all done!!!!
					
					setAlert("Thanks for your time!",null,null);
				}

			}
			
			
		}else{//demo mode
			
			setAlert("Go again? Just click the [space] bar, otherwise the real test will begin in:<br>",
			10,
			function(){window.location.assign(goTo+"?pid="+pid+"&group="+group)});
			
		}
	}
				
}


function sart_start(){
	
	window.onkeypress = keypress;
	
	if(DEMO){
	
		reset();	
		countDown(5);
	
	}else if(run<RUNS){
		
		reset();
		
		s = state[run];
		results.push({user:pid, state:s, runs:RUNS, run:run, date:new Date(),type:TYPE,omit:OMIT,reps:REPS,mask_ms:MASK_DURATION, sym_ms:SYM_DURATION});
		
		countDown(5);
			
	}
	
}

//window.onload =function(){
function loadSART(){
	
	preloadImages();
	
	ajax_rq("array_off.txt"); 
	
	var urlp = getUrlVars();
	
	
	if(Object.entries(urlp).length === 0 )
	{	alert("something went wrong, you shouldn't be here without a pid!");
		return;
	}
	
		
	pid = urlp.pid;
	console.log(pid);

	group = urlp.group;
	console.log(group);

	
	displayBx = document.getElementById('displayBx');
	
	
	/*load random states for each run*/    
	state = loadStates(RUNS);
		
	state = shuffle(shuffle(shuffle(state)));
	
	console.log(state);
	
	//clear results, init run
	run = 0;
	results=[];
	
	
	if(DEMO){
		document.getElementById("instructions").style.display='block';
		
		//document.getElementById("omit").innerHTML = OMIT;
		appendImageElement(OMIT,document.getElementById("omit"));
		
		document.getElementById("correct").innerHTML = MASK;
		document.getElementById("wrong").innerHTML = MASK;
	}
	
	
}

function appendImageElement(omarray,pelem){
	var i=0;
	for(i=0; i<omarray.length; i++)
	{
		var node = document.createElement("DIV");
		node.className = 'omit';
		pelem.appendChild(node);
		setImageOverride(omarray[i],node);
	}
	
}

function keypress(e){
	
	if(e.key==KEY){
		
		if(!KEY_EN && !hTimer)  /*test hasn't started yet*/
		{
			sart_start();
			console.log("starting test");
			
		}else if(KEY_EN && hTimer){ /*test is running*/
				
			KEY_EN = false;
			var now = new Date();
			RT = now.getTime()-RT0;
			
			CLICKED = 1;
			
			console.log(RT);
			feedback(sym);	
			
			
		}
		
	}else if( (e.key=="q" || e.key=="Q")  && hTimer){
				
		sart_quit();
		console.log("aborting test");
	}	
		
}

window.onkeypress = keypress;
