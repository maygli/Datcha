class HttpProcessor{
    static async loadData(theURL, isText, theParameters=null)
    {
        let anURL = theURL;
        if( theParameters ){
            anURL = anURL + "?" + new URLSearchParams(theParameters).toString();
        }
        let response = await fetch(anURL);
        if( !response.ok ){
            throw new Error('Server response error', {"response_code":response.status,"response_message":response.text()});
        }
        if( isText ){
            let aText = await response.text();
            return aText;
        }
        let aJson = await response.json();
        return aJson;
    }

    static async sendFormData(theURL, theForm){
        let aFormData = new FormData(theForm);
        let aData = {};
        for (let aTuple of aFormData.entries()) aData[aTuple[0]] = aTuple[1];
        let aRes = await this.sendPostJSON(theURL,aData); 
        return aRes;
    }

    static async sendPostJSON(theURL, theJSON){
        let aSendText = JSON.stringify(theJSON);
        let aResponse = await fetch(theURL, {
            method: "POST",
            headers: {
              'Content-Type': 'application/json; charset=UTF-8'
            },
            body: aSendText,
        });
        if( !aResponse.ok ){
            throw new Error('Failed to upload form data');
        }
        return aResponse.statusText;
    }

    static async postFormData(theURL, theData)
    {
        let aResponse = await fetch(theURL, {
            method:"POST",
            body: theData,
        });
        if( !aResponse.ok ){
            throw new Error('Failed to upload form data');
        }
    }

    static uploadFile( theFile, theRequest, theParams, theLoadProgressCB, theLoadCompleteCB, theLoadErrCB, theLoadAbortCB ){
        theParams.append("file", theFile);
        let aReq = new XMLHttpRequest();
        aReq.upload.addEventListener("progress", theLoadProgressCB, false);
        aReq.addEventListener("load", theLoadCompleteCB, false);
        aReq.addEventListener("error", theLoadErrCB, false);
        aReq.addEventListener("abort", theLoadAbortCB, false);
        aReq.open("POST", theRequest); 
        aReq.send(theParams);
    }
}

export {HttpProcessor};