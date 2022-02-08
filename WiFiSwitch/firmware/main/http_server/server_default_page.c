/*
 * Copyright (c) 2022 Maygli (mmaygli@gmail.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

const char HTTP_DEFAULT_PAGE[]="<!DOCTYPE html>\n\
<html lang=\"en\">\n\
<head>\n\
    <title>Manta Ray GRBL controller</title>\n\
    <style>\n\
        .manta_title {\n\
            text-align: center;\n\
            background: #2980b9;\n\
            color: #FFFFFF;\n\
            }\n\
        .collapsible {\n\
            background-color: #3F3F3F;\n\
            color: #ffffff;\n\
            width: 100%;\n\
            border: none;\n\
            text-align: left;\n\
            outline: none;\n\
        }\n\
        .active, .collapsible:hover, .button:hover {\n\
            background-color: #35a6f0;\n\
        }\n\
        #status {\n\
            display: inline-block;\n\
            padding: 2px;\n\
        }\n\
        .button{\n\
            display: inline-block;\n\
            padding: 2px;\n\
            background-color: #3F3F3F;\n\
            color: #ffffff;\n\
            border: 1px solid #000000;\n\
            border-radius: 4px;\n\
        }\n\
        .input_file{\n\
            display: none;\n\
        }\n\
    /* The Modal (background) */\n\
        .modal {\n\
            display: none; /* Hidden by default */\n\
            position: fixed; /* Stay in place */\n\
            z-index: 1; /* Sit on top */\n\
            padding-top: 10%; /* Location of the box */\n\
            left: 0;\n\
            top: 0;\n\
            width: 100%; /* Full width */\n\
            height: 100%; /* Full height */\n\
            overflow: auto; /* Enable scroll if needed */\n\
            background-color: rgb(0,0,0); /* Fallback color */\n\
            background-color: rgba(0,0,0,0.4); /* Black w/ opacity */\n\
        }\n\
\n\
            /* Modal Content */\n\
        .upload_progress {\n\
            background-color: #fefefe;\n\
            margin: auto;\n\
            padding: 2px;\n\
            padding-top: 0;\n\
            border: 1px solid #888;\n\
            width: 80%;\n\
        }\n\
\n\
        .close {\n\
            float: right;\n\
            font-weight: bold;\n\
        }\n\
\n\
        .error_msg{\n\
            background-color: #f8cbad;\n\
        }\n\
\n\
        .success_msg{\n\
            background-color: seagreen;\n\
        }\n\
\n\
        .normal_msg{\n\
            background-color: transparent;\n\
        }\n\
    </style>\n\
    <script>\n\
        function onExpandDesc(){\n\
            var aButton = document.getElementById(\"collapsible\");\n\
            var aContent = document.getElementById(\"content\");\n\
            if(aContent.hidden === true ){\n\
               aContent.hidden = false;\n\
               aButton.innerHTML = \"- Firmware file decription\";\n\
            }\n\
            else{\n\
               aContent.hidden = true;\n\
               aButton.innerHTML = \"+ Firmware file decription\";\n\
            }\n\
        }\n\
       \n\
        function onUploadFile(){\n\
            aModal = document.getElementById(\"modal\");\n\
            aBtnClose = document.getElementById(\"button_close\");\n\
            aBtnClose.style.display = \"none\"\n\
            aModal.style.display=\"block\"\n\
            var aStatus = document.getElementById(\"status\");\n\
            aStatus.className = \"normal_msg\";\n\
            var aFile = document.getElementById(\"file\").files[0];\n\
            aFilenameInfo = document.getElementById(\"filename_info\");\n\
            aFilenameInfo.innerHTML = \"File:  <b>\" + aFile.name + \"</b>\";\n\
            if( aFile.name.length > 31 ){\n\
                aStatus.innerHTML = \"Can't upload file. Filename lenght shouldn't exceed 32 characters.\";\n\
                aStatus.className = \"error_msg\";\n\
                aBtnClose = document.getElementById(\"button_close\");\n\
                aBtnClose.style.display = \"block\"\n\
                return false;\n\
            }\n\
            var aClear = document.getElementById(\"clear\").checked;\n\
            var aFormData = new FormData();\n\
            aFormData.append(\"clear\",aClear);\n\
            aFormData.append(\"file\", aFile);\n\
            var aReq = new XMLHttpRequest();\n\
            aReq.upload.addEventListener(\"progress\", onLoadProgress, false);\n\
            aReq.addEventListener(\"load\", onLoadComplete, false);\n\
            aReq.addEventListener(\"error\", onLoadError, false);\n\
            aReq.addEventListener(\"abort\", onLoadAbort, false);\n\
            aReq.open(\"POST\", \"upload\"); \n\
            aReq.send(aFormData);\n\
\n\
  // alert(file.name+\" | \"+file.size+\" | \"+file.type);\n\
            return false;\n\
        }\n\
\n\
        function onLoadProgress(event) {\n\
            aProgressBar = document.getElementById(\"progress_bar\")\n\
            aProgressBar.style.display=\"block\";\n\
            aLoadTotal = document.getElementById(\"loaded_n_total\");\n\
            aLoadTotal.innerHTML = \"Uploaded \" + event.loaded + \" bytes of \" + event.total;\n\
            var aPercent = (event.loaded / event.total) * 100;\n\
            aProgressBar.value = Math.round(aPercent);\n\
            aStatus = document.getElementById(\"status\");\n\
            aStatus.innerHTML = Math.round(aPercent) + \"% uploaded... please wait\";\n\
//            aStatus.style.backgroundColor = \"transparent\";\n\
        }\n\
\n\
        function onLoadComplete(event) {\n\
            document.getElementById(\"progress_bar\").style.display=\"none\";\n\
            aStatus = document.getElementById(\"status\");\n\
            aStatus.innerHTML = event.target.responseText;\n\
            if(  event.target.status == 200 ){\n\
                aStatus.className = \"success_msg\";\n\
            }\n\
            else{\n\
                aStatus.className = \"error_msg\";\n\
            }\n\
            aBtnClose = document.getElementById(\"button_close\");\n\
            aBtnClose.style.display = \"block\"\n\
        }\n\
\n\
        function onLoadError(event) {\n\
            document.getElementById(\"progress_bar\").style.display=\"none\";\n\
            aStatus = document.getElementById(\"file\").value=null;\n\
            aStatus = document.getElementById(\"status\");\n\
            aStatus.innerHTML = \"Upload Failed\";\n\
            aStatus.className = \"error_msg\";\n\
            aBtnClose = document.getElementById(\"button_close\");\n\
            aBtnClose.style.display = \"block\"\n\
        }\n\
\n\
        function onLoadAbort(event) {\n\
            document.getElementById(\"progress_bar\").style.display=\"none\";\n\
            aStatus = document.getElementById(\"file\").value=null;\n\
            aStatus.innerHTML = \"Upload Aborted\";\n\
            aStatus.className = \"error_msg\";\n\
            aBtnClose = document.getElementById(\"button_close\");\n\
            aBtnClose.style.display = \"block\"\n\
        }\n\
\n\
       function onCloseClick(){\n\
            aModal = document.getElementById(\"modal\");\n\
            aModal.style.display = \"none\";\n\
       }\n\
      \n\
    </script>\n\
</head>\n\
<body>\n\
    <h1 id=\"manta_title\" class=\"manta_title\">Manta Ray GRBL controller</h1>\n\
    <div><h2>Please select firmware file to upload to controller</h2></div>\n\
    <p/>\n\
    <button type=\"button\" class=\"collapsible\" id=\"collapsible\" onclick=\"onExpandDesc()\">+ Firmware file description</button>\n\
    <div id=\"content\" hidden=\"true\">\n\
        <hr/>\n\
        <p/>\n\
             <h3>Firmware file is a zip file which can contains:</h3>\n\
        <p/>   \n\
        <table>\n\
	    <tr><td><b>controller.bin</b></td><td>Grbl controller firmware</td></tr>\n\
	    <tr><td><b>grbl.bin</b></td><td>Grbl board firmware</td></tr>\n\
	    <tr><td><b>config.json</b></td><td>Grbl controller configuration file</td></tr>\n\
	    <tr><td><b>*<b></td><td>any other html/css/js files related to controller web application</td></tr> \n\
        </table>\n\
        <h4>Please pay attantion, filename length shouldn't exceed 32 characters</h4>\n\
        <hr>\n\
    </div>\n\
    <p/>\n\
    <div>\n\
        <input type=\"checkbox\" name=\"Clear\" id=\"clear\" class=\"button\" checked/>\n\
        <label for=\"clear\">Clear controller memory before upload</label>\n\
        <br/>            \n\
        <input type=\"file\" name=\"name\" class=\"input_file\" id=\"file\" onchange=\"onUploadFile()\">\n\
        <label for=\"file\" class=\"button\">Select file</label>\n\
        <br/>\n\
        <div id=\"modal\" class=\"modal\">\n\
            <div id=\"upload_progress\" class=\"upload_progress\">\n\
                <h2 id=\"upload_title\" class=\"manta_title\">Uploading firmware</h2>\n\
                <p id=\"filename_info\">File:</p>\n\
                <progress id=\"progress_bar\" value=\"0\" max=\"100\" style=\"display:none\"></progress> \n\
                <h3 id=\"status\"></h3>\n\
                <p id=\"loaded_n_total\"></p>             \n\
                <p>&nbsp<span class=\"button close\" id=\"button_close\" onclick=\"onCloseClick()\">Close</span></p>\n\
            </div>\n\
        </div>\n\
    </div>\n\
</body>\n\
</html>\n\
";
