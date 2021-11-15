/*global config*/

var KEY = " ";          //the main input key user reacts with e.g. spacebar
//var MASK = "&#x29BB;";
var MASK = "&#8709;";  //8709   8855


/*Symbols:   HTML codes from:  https://www.rapidtables.com/web/html/html-codes.html*/

var SQUARE = "&#9723;";
var CIRCLE = "&#9675;";	
var DIAMOND = "&#9671;";
var TRIANGLE = "&#9651;";	
var HEART = "&#9825;";
var STAR = "&#9734;";
var SUN = "&#9965;"

var SHAPES= new Array(SQUARE, CIRCLE, DIAMOND, TRIANGLE, HEART, STAR, SUN);



var ORANGE ="&#127818;";
var TOMATO = "&#127813;";
var LEMON = "&#127819;";
var APPLE = "&#127822;";
var PEAR = "&#127824;";
var GRAPE = "&#127815;";
var CHERRY = "&#127826;";
var MELON = "&#127817;";
var PEACH = "&#127825;";
var STAWBERRY = "&#127827;";

var FRUITS = new Array(ORANGE,TOMATO, LEMON,APPLE, PEAR, GRAPE,CHERRY,MELON,PEACH,STAWBERRY);



var NUMBERS = new Array("1","2","3","4","5","6","7","8","9","0");

var LETTERS = new Array("A", "h", "G","k","p", "Q","x","Y","z","S");
	


/*Display geometry and timings; note font size controlled in test.css file */
var XY_SHIFT_MAX = 600;
var MASK_DURATION = 750;
var SYM_DURATION =  250;
var DEFAULT_CLR = "white";
var DEFAULT_BKG = "black";


var pid;
var group;



var RUNS = 2; /*number of runs i.e. realisations of the type of test:min(RUNS) = 2 and an even number*/
var run;   	  /*current run*/
var state; 	  /*current state [ctl, exp]*/


var REPS = 2;   /*default number of repetitions of symbol set per run*/

var KEY_EN;		/*KEY enabled flag*/
var RT;			/*current reaction time*/
var RT0;        /*base time; for measuring delta RT*/

var displayBx;  /*display box html element*/
var hTimer;     /*handle to timer*/
var count;      /*loop counter*/
var halfway;    /*half of loop counter*/
var halfwayflg;
var symbols;    /*symbols array e.g. FRUITS*/

var results;    /*Overall results*/
var result;     /*result from latest 'run'*/

var sym;        /*current (sym)bol*/

var CLICKED;

var pTimer;		/*alert message timer*/


/*scores*/
var FP;
var TP;
var FN;
var TN;



function getUrlVars() {
    var vars = {};
    var parts = window.location.href.replace(/[?&]+([^=&]+)=([^&]*)/gi, function(m,key,value) {
        vars[key] = value;
    });
    return vars;
}



/*can this be sent via ajax?*/
function download(filename, text) {
    var pom = document.createElement('a');
    pom.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
    pom.setAttribute('download', filename);

    if (document.createEvent) {
        var event = document.createEvent('MouseEvents');
        event.initEvent('click', true, true);
        pom.dispatchEvent(event);
    }
    else {
        pom.click();
    }
}


function upload(pathtoserver, text) {
    
    var data = text;//encodeURIComponent(text);
   
   var xhttp = new XMLHttpRequest();
   
   xhttp.onload = function () {
      if (xhttp.status == 200) {
        document.body.style.backgroundColor = "purple"; 
      } else {
        alert('Upload error!');
      }
    };
   
  xhttp.open("POST",pathtoserver, true);
  xhttp.send(data);

 
}



/*ajax function*/		
function ajax_rq(filename) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
     //do nothing!
     //document.getElementById("demo").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", filename, true);
  xhttp.send();
}



/**
 * Shuffles array in place.
 * @param {Array} a items An array containing the items.
 */
function shuffle(a) {
    var j, x, i;
    for (i = a.length - 1; i > 0; i--) {
        j = Math.floor(Math.random() * (i + 1));
        x = a[i];
        a[i] = a[j];
        a[j] = x;
    }
    return a;
}



function setAlert(msg,n,action){
	
	var elem = document.getElementById('alertBx');
	elem.style.display = 'block';
	
	if(n==null && action==null)
	{	
		elem.innerHTML = msg;
	
	}else{  //countdown to action
		
		if(n>0)
		{
			elem.innerHTML = msg+"<p style='font-size:60pt'>"+n.toString()+"</p>";
			
			pTimer=setTimeout(function(){setAlert(msg,n-1,action);},1000);
		
		}else{
			
			if(action!=null)
			action();
		}
				
	}	
	
}



function clearAlert(){
	
	clearTimeout(pTimer);
	pTimer=null;
	
	var elem = document.getElementById('alertBx');
	
	elem.innerHTML = "";
	elem.style.display = 'none';
	
	console.log("alert cleared");
	
}
