var fields = {
  o: 'empty', //top 1 field
  u: 'empty', //bottom 1 field
  lo: 'empty', //2 field
  lu: 'empty', //3 field
  rm: 'empty' //5 field
};

var testH = 0;
var testM = 0;

function mousePressed() {
  if ((testM + 5) == 60) {
    testM = 0;
    testH += 1;
  } else {
    testM += 5;
  }
  if (testH == 12) {
    testH = 0;
  }
}

function setup() {
  createCanvas(640, 400);
  background(0);
  //print(fields.o,fields.u,fields.lo,fields.lu,fields.rm);
  //print(fields.o,fields.u,fields.lo,fields.lu,fields.rm);
}

function draw() {
  clearField();
  getTime();
  showTime();
  frameRate(20);
  stroke(0);
  strokeWeight(5);
  line(240, 0, 240, 400);
  line(160, 0, 160, 160);
  line(0, 160, 240, 160);
  line(160, 80, 240, 80);
  //textSize(30);
  //text(testH + ":" + testM, 500, 300);
}

function getTime() {
  var hr = hour();
  var mn = minute()/5;


  if (hr >= 12) {
    hr -= 12; //max val of hr is 11 not 23
  }
  if (hr >= 5) {
    fields.rm = 'hr';
    hr -= 5;
  }
  if (hr >= 3) {
    fields.lu = 'hr';
    hr -= 3;
  }
  if (hr >= 2) {
    fields.lo = 'hr';
    hr -= 2;
  }
  if (hr >= 1) {
    fields.o = 'hr';
    hr -= 1;
  }
  if (mn >= 5) {
    if (fields.rm == 'hr') {
      fields.rm = 'both';
    } else {
      fields.rm = 'mn';
    }
    mn -= 5;
  }
  if (mn >= 3) {
    if (fields.lu == 'hr') {
      fields.lu = 'both';
    } else {
      fields.lu = 'mn';
    }
    mn -= 3;
  }
  if (mn >= 2) {
    if (fields.lo == 'hr') {
      fields.lo = 'both';
    } else {
      fields.lo = 'mn';
    }
    mn -= 2;
  }
  if (mn >= 1) {
    fields.u = 'mn';
  }

}

function clearField() {
  fields.o = 'empty';
  fields.u = 'empty';
  fields.lo = 'empty';
  fields.lu = 'empty';
  fields.rm = 'empty';
}

function getColor(request) {
  if (request == 'hr') {
    return color(255, 0, 0);
  } else if (request == 'mn') {
    return color(0, 255, 0);
  } else if (request == 'both') {
    return color(0, 0, 255);
  } else {
    return color(255, 255, 255);
  }

}

function showTime() {
  noStroke();
  fill(getColor(fields.o));
  rect(161, 0, 239, 79);
  fill(getColor(fields.u));
  rect(161, 81, 239, 159);
  fill(getColor(fields.lo));
  rect(0, 0, 159, 159);
  fill(getColor(fields.lu));
  rect(0, 161, 239, 400);
  fill(getColor(fields.rm));
  rect(241, 0, 640, 400);

}
