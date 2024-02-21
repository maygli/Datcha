import { UnitConverter } from "./unit_conversion.js";
import {Updater} from "./updater.js"
import { BaseController } from "./controllers/base_controller.js"

class UnitWidget extends BaseController{
    constructor(theEl){
        super(theEl);
        this._m_ValEl = theEl.querySelector("#wid_value");
        this._m_UnitEl = theEl.querySelector("#wid_unit");
        this._m_BaseElement.addEventListener("click", this._onClick.bind(this)); 
        let anUpdater = Updater.getUpdater();
        anUpdater.registerClient("meteo_state",this._update.bind(this));
    }

    setValue(theValue, theUnitIndx){
        this._m_ValEl.setAttribute("unit_indx", theUnitIndx);
        this._m_ValEl.setAttribute("wid_val", theValue);
        this._displayValue();
    }

    _onClick(){
        let aCurrUnitIndx = parseInt(this._m_UnitEl.getAttribute("unit_indx"));
        let anUnitsCnt = this._getUnitsCount();
        let aNewUnitIndx = aCurrUnitIndx+1;
        if( aNewUnitIndx >= anUnitsCnt ){
            aNewUnitIndx = 0;
        }
        this._m_UnitEl.setAttribute("unit_indx", aNewUnitIndx);
        this._displayValue();
    }

    _displayValue(){
        let aNewUnitIndx = parseInt(this._m_UnitEl.getAttribute("unit_indx"));
        let aCurrUnitIndx = parseInt(this._m_ValEl.getAttribute("unit_indx"));
        let aVal = parseFloat(this._m_ValEl.getAttribute("wid_val"));
        let aNewVal = this._convertValue(aVal,aCurrUnitIndx,aNewUnitIndx);
        this._m_ValEl.innerHTML = aNewVal.toFixed(this._getDefaultPrecision(aNewUnitIndx));
        this._m_UnitEl.setAttribute("unit_indx", aNewUnitIndx);
        let aNewUnit = this._getUnitSimbol(aNewUnitIndx);
        this._m_UnitEl.innerHTML = aNewUnit;
    }

    _getUnitsCount(){
        return UnitConverter.getTemperatureUnitsCount();
    }

    _getUnitSimbol(theIndx){
        return UnitConverter.getTemperatureUnit(theIndx);
    }

    _convertValue(theValue, theUnitIndxFrom, theUnitIndxTo){
        return UnitConverter.convertTemperature(theValue,theUnitIndxFrom,theUnitIndxTo);
    }

    _getDefaultPrecision(theUnitIndx){
        return UnitConverter.getTemperaturePrecision(theUnitIndx);
    }

    _update(theURL, theData){
        if( theData.is_temp ){
            this.show()
        }
        else{
            this.hide();
        }
        this.setValue(theData.temp, theData.temp_unit_index);
    }
}

class PressureWidget extends UnitWidget{
    constructor(theEl){
        super(theEl);
    }

    _getUnitsCount(){
        return UnitConverter.getPressureUnitsCount();
    }

    _getUnitSimbol(theIndx){
        return UnitConverter.getPressureUnit(theIndx,"");
    }

    _convertValue(theValue, theUnitIndxFrom, theUnitIndxTo){
        return UnitConverter.convertPressure(theValue,theUnitIndxFrom,theUnitIndxTo);
    }

    _getDefaultPrecision(theUnitIndx){
        return UnitConverter.getPressurePrecision(theUnitIndx);        
    }

    _update(theURL, theData){
        if( theData.is_pressure ){
            this.show()
        }
        else{
            this.hide();
        }
        console.log("Update calld", theURL, theData)
        this.setValue(theData.pressure, theData.pressure_unit_index);
    }

}

class HumidityWidget extends UnitWidget
{
    constructor(theEl){
        super(theEl);
    }

    _getUnitsCount(){
        return 1;
    }

    _getUnitSimbol(theIndx){
        return "%";
    }

    _convertValue(theValue, theUnitFrom, theUnitTo){
        return theValue;
    }

    _getDefaultPrecision(theUnitIndx){
        return 0;
    }

    _update(theURL, theData){
        if( theData.is_humidity ){
            this.show()
        }
        else{
            this.hide();
        }
        this.setValue(theData.humidity, theData.humidity_unit_index);
    }

}

export function onInit()
{
    let aTempEl = document.querySelector("#temp_wid");
    let aTempWdg = new UnitWidget(aTempEl);
    let aPressureEl = document.querySelector("#pressure_wid");
    let aPressureWdg = new PressureWidget(aPressureEl);
    let aHumEl = document.querySelector("#humidity_wid");
    let aHumidityWdg = new HumidityWidget(aHumEl);
}
