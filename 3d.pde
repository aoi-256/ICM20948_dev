import processing.serial.*;

Serial myPort;
float angleX, angleY, angleZ;

void setup() {
  size(1280, 720, P3D);
  myPort = new Serial(this, Serial.list()[3], 115200); // 正しいポート番号に変更
  myPort.bufferUntil('\n');
  
}

void draw() {
  background(200);
  lights();
  
　　// 3ðモデルの描画
  translate(width/2, height/2, 0);
  rotateX(angleX);
  //rotateY(angleY); ヨー方向の有効化
  rotateY(0);//ヨー方向の無効化

  rotateZ(angleZ);
  drawRectangularPrism(100, 50, 150); // 幅、高さ、奥行きを指定
  
}

void serialEvent(Serial myPort) {
  String inString = myPort.readStringUntil('\n');
  if (inString != null) {
    inString = trim(inString);
    String[] angles = split(inString, ' ');
    if (angles.length == 3) {
      angleX = -radians(float(angles[0]) / 100);
      angleY = radians(float(angles[2]) / 100);
      angleZ = -radians(float(angles[1]) / 100);
      
      println("X = " + angleX + " " + "Y = " + angleY + " "  + "Z = " + angleZ);
    }
  }
}

void drawRectangularPrism(float w, float h, float d) {
  beginShape(QUADS);
  
  // 前面
  
  fill(255, 0, 0);
  vertex(-w/2, -h/2, d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, h/2, d/2);
  vertex(-w/2, h/2, d/2);
  
  // 背面
  
  fill(200);
  vertex(-w/2, -h/2, -d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, -d/2);
  
  // 左側面
  vertex(-w/2, -h/2, d/2);
  vertex(-w/2, -h/2, -d/2);
  vertex(-w/2, h/2, -d/2);
  vertex(-w/2, h/2, d/2);
  
  // 右側面
  vertex(w/2, -h/2, d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(w/2, h/2, -d/2);
  vertex(w/2, h/2, d/2);
  
  // 上面
  vertex(-w/2, -h/2, d/2);
  vertex(w/2, -h/2, d/2);
  vertex(w/2, -h/2, -d/2);
  vertex(-w/2, -h/2, -d/2);
  
  // 底面
  vertex(-w/2, h/2, d/2);
  vertex(w/2, h/2, d/2);
  vertex(w/2, h/2, -d/2);
  vertex(-w/2, h/2, -d/2);
  
  endShape();
}
