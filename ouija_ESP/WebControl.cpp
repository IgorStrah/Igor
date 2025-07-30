// WebControl.cpp
#include "WebControl.h"
#include <LittleFS.h>

static ESP8266WebServer* _server;
static String* _phrases;
static int _phraseCount;
static String* _debugLog;
static File fsUploadFile;

void loadPhrasesFromFile() {
  File file = LittleFS.open("/phrases.txt", "r");
  if (!file) return;

  int i = 0;
  while (file.available() && i < _phraseCount) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      _phrases[i++] = line;
    }
  }
  file.close();
}

void handleSavePhrases() {
  if (!_server->hasArg("content")) {
    _server->send(400, "text/plain", "Missing content");
    return;
  }

  File file = LittleFS.open("/phrases.txt", "w");
  if (!file) {
    _server->send(500, "text/plain", "Failed to open file");
    return;
  }

  String content = _server->arg("content");
  file.print(content);
  file.close();
  _server->send(200, "text/plain", "Phrases saved");

  int i = 0;
  int from = 0;
  while (i < _phraseCount) {
    int nl = content.indexOf('\n', from);
    if (nl == -1) nl = content.length();
    String line = content.substring(from, nl);
    line.trim();
    _phrases[i++] = line;
    from = nl + 1;
    if (from >= content.length()) break;
  }
}

void handleFileList() {
  String output = "[";
  Dir dir = LittleFS.openDir("/");
  while (dir.next()) {
    if (output != "[") output += ',';
    output += "{\"name\":\"" + dir.fileName() + "\",\"size\":" + String(dir.fileSize()) + "}";
  }
  output += "]";
  _server->send(200, "application/json", output);
}

void handleFileUpload() {
  HTTPUpload& upload = _server->upload();
  if (upload.status == UPLOAD_FILE_START) {
    String filename = "/" + upload.filename;
    fsUploadFile = LittleFS.open(filename, "w");
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (fsUploadFile) fsUploadFile.write(upload.buf, upload.currentSize);
  } else if (upload.status == UPLOAD_FILE_END) {
    if (fsUploadFile) fsUploadFile.close();
  }
}

void handleDeleteFile() {
  if (_server->hasArg("name")) {
    LittleFS.remove(_server->arg("name"));
    _server->send(200, "text/plain", "Deleted");
  } else {
    _server->send(400, "text/plain", "Missing file name");
  }
}

void handleEditFile() {
  if (_server->hasArg("name") && _server->hasArg("content")) {
    File f = LittleFS.open(_server->arg("name"), "w");
    f.print(_server->arg("content"));
    f.close();
    _server->send(200, "text/plain", "Edited");
  } else {
    _server->send(400, "text/plain", "Missing parameters");
  }
}

void setupWebServer(ESP8266WebServer& server, bool& wifiConnected, String phrases[], int uidCount, String& debugLog) {
  _server = &server;
  _phrases = phrases;
  _phraseCount = uidCount;
  _debugLog = &debugLog;

  if (!LittleFS.begin()) return;

  loadPhrasesFromFile();

  server.on("/", HTTP_GET, [&]() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>ESP Interface</title></head><body>";
    html += "<h2>ESP Web Interface</h2>";
    html += "<ul>";
    html += "<li><a href='/edit_phrases'>Edit phrases</a></li>";
    html += "<li><a href='/debug'>Debug log</a></li>";
    html += "</ul><hr>";
    html += "<form method='POST' action='/upload' enctype='multipart/form-data'>";
    html += "<input type='file' name='upload'><input type='submit' value='Upload'>";
    html += "</form><div id='filelist'></div>";
    html += "<script>";
    html += "async function listFiles() { const res = await fetch('/list'); const files = await res.json(); let html = '<ul>';";
    html += "files.forEach(file => { html += `<li><a href='${file.name}' target='_blank'>${file.name}</a> [<a href='#' onclick=\\\"editFile('${file.name}')\\\">edit</a>] [<a href='/delete?name=${file.name}'>delete</a>]</li>`; });";
    html += "html += '</ul>'; document.getElementById('filelist').innerHTML = html; }";
    html += "function editFile(name) { fetch(name).then(r => r.text()).then(text => { const newText = prompt('Edit file content:', text); if (newText !== null) { fetch('/edit', { method: 'POST', headers: { 'Content-Type': 'application/x-www-form-urlencoded' }, body: 'name=' + encodeURIComponent(name) + '&content=' + encodeURIComponent(newText) }).then(() => listFiles()); } }); } listFiles();";
    html += "</script></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/edit_phrases", HTTP_GET, [&]() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Edit Phrases</title></head><body>";
    html += "<h2>Edit Phrases</h2>";
    html += "<form method='POST' action='/save_phrases'>";
    html += "<textarea name='content' rows='12' cols='40'>";
    for (int i = 0; i < _phraseCount; i++) {
      html += _phrases[i] + "\n";
    }
    html += "</textarea><br>";
    html += "<input type='submit' value='Save'>";
    html += "</form></body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/save_phrases", HTTP_POST, handleSavePhrases);
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/upload", HTTP_POST, [&]() {
    server.send(200, "text/plain", "Uploaded");
  }, handleFileUpload);
  server.on("/delete", HTTP_GET, handleDeleteFile);
  server.on("/edit", HTTP_POST, handleEditFile);

  server.on("/debug", HTTP_GET, [&]() {
    String html = "<!DOCTYPE html><html><head><meta charset='utf-8'><title>Debug Log</title></head><body>";
    html += "<h2>Debug Log</h2><pre>" + *_debugLog + "</pre>";
    html += "<meta http-equiv='refresh' content='2'>";
    html += "</body></html>";
    server.send(200, "text/html", html);
  });

  server.on("/clearlog", HTTP_GET, [&]() {
    *_debugLog = "";
    server.send(200, "text/plain", "Log cleared");
  });

  server.onNotFound([&]() {
    String path = server.uri();
    if (LittleFS.exists(path)) {
      File file = LittleFS.open(path, "r");
      server.streamFile(file, "text/plain");
      file.close();
    } else {
      server.send(404, "text/plain", "File Not Found");
    }
  });

  server.begin();
}

void handleWebLoop() {
  if (_server) _server->handleClient();
}
