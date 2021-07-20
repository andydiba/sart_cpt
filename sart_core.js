
/** Overwrite these in html file header to make test SART/CPT, DEMO or real and adjust symbols*/

var TYPE = "sart";   /*cpt*/

var DEMO = true;     

var MYSYMBOLS = FRUITS;

var OMIT = new Array(ORANGE, LEMON);   //OMIT for sart, ACCEPT for CPT



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



function setRandSymbol(){

	if(count>0){
		
		
		checkPrevious(sym);
		logRT();  //logs previous [RT , sym] 
				
		var now = new Date();
		RT0 = now.getTime();
		RT = 0;

		//pop random number from array and assign to current symbol -> 'sym'
		sym = symbols.pop();

		var rd1 =  Math.random();
		var rd2 =  Math.random();

		//Randomize Font and Position
		var font = "f"+(Math.floor(5*rd1)).toString();  //[0,4]

		var pos_x = ((rd1-0.5)*XY_SHIFT_MAX).toString()+"px";
		var pos_y = ((rd2-0.5)*XY_SHIFT_MAX).toString() +"px";
		
		displayBx.className = font;
		displayBx.style.left = pos_x;
		displayBx.style.top = pos_y;
		displayBx.style.color = DEFAULT_CLR;
		
		displayBx.innerHTML = sym;


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
			if(state[run]=="exp")
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
				download("data"+pid, JSON.stringify(results));	
				upload("uploader.txt",pid+"$"+JSON.stringify(results));
	
				
				if(goTo!=null){
					
					setAlert("Finished this test. Moving on to practice session of next type<br>",
					20,
					function(){window.location.assign(goTo+"?pid="+pid)});
					
				}else{  //all done!!!!
					
					setAlert("Thanks for your time!",null,null);
				}

			}
			
			
		}else{//demo mode
			
			setAlert("Go again? Just click the [space] bar, otherwise the real test will begin in:<br>",
			10,
			function(){window.location.assign(goTo+"?pid="+pid)});
			
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
	
	
	ajax_rq("array_off.txt"); 
	
	var urlp = getUrlVars();
	
	
	if(Object.entries(urlp).length === 0 )
	{	alert("something went wrong, you shouldn't be here without a pid!");
		return;
	}
	
		
	pid = urlp.pid;
	console.log(pid);
	
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
		document.getElementById("omit").innerHTML = OMIT;
		document.getElementById("correct").innerHTML = MASK;
		document.getElementById("wrong").innerHTML = MASK;
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
