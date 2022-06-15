const UNIT_TEMP_INDX_C=0;
const UNIT_TEMP_INDX_F=1;
const UNIT_TEMP_INDX_K=2;

const UNIT_TEMP_F_ZERO=32;
const UNIT_TEMP_K_ZERO=273.15;
const UNIT_TEMP_C_TO_F_DIV=1.8;

const UNIT_PRESS_INDX_MMHG=0;
const UNIT_PRESS_INDX_KPA=1;
const UNIT_PRESS_INDX_BAR=2;
const UNIT_PRESS_INDX_AT=3;
const UNIT_PRESS_INDX_PSI=4;

const UNIT_MMHG_TO_KPA=0.133322;
const UNIT_MMHG_TO_BAR=0.00133322;
const UNIT_MMHG_TO_PSI=0.0193368;
const UNIT_MMHG_TO_ATM=0.00131579;

class UnitConverter{

    static _s_TempUnits = ["&deg;C", "&deg;F", "K"];
    static _s_PressureUnits = ["mm Hg", "kPa", "bar", "atm", "psi"];

    static getTemperatureUnitsCount()
    {
        return UnitConverter._s_TempUnits.length;
    }

    static getTemperatureUnit(theIndx, theLang)
    {
        if( ( theIndx >= 0 ) && ( theIndx < UnitConverter._s_TempUnits.length ) ){
            return UnitConverter._s_TempUnits[theIndx];
        }
        return "";
    }

    static convertTemperature(theValue, theIndexFrom, theIndexTo)
    {
        let aValue = theValue;
        if( theIndexFrom == theIndexTo )
            return aValue;           
        if( theIndexFrom == UNIT_TEMP_INDX_C ){
            if( theIndexTo == UNIT_TEMP_INDX_F ){
                aValue = theValue*UNIT_TEMP_C_TO_F_DIV+UNIT_TEMP_F_ZERO;
            }
            if( theIndexTo == UNIT_TEMP_INDX_K ){
                aValue = theValue + UNIT_TEMP_K_ZERO;
            }
        }
        if( theIndexFrom == UNIT_TEMP_INDX_F ){
            if( theIndexTo == UNIT_TEMP_INDX_C ){
                aValue = (theValue-UNIT_TEMP_F_ZERO)/UNIT_TEMP_C_TO_F_DIV;
            }
            if( theIndexTo == UNIT_TEMP_INDX_K ){
                aValue = (theValue-UNIT_TEMP_F_ZERO)/UNIT_TEMP_C_TO_F_DIV + UNIT_TEMP_K_ZERO;
            }
        }
        if( theIndexFrom == UNIT_TEMP_INDX_K ){
            if( theIndexTo == UNIT_TEMP_INDX_C ){
                aValue = theValue - UNIT_TEMP_K_ZERO;
            }
            if( theIndexTo == UNIT_TEMP_INDX_F ){
                aValue = (theValue - UNIT_TEMP_K_ZERO)*UNIT_TEMP_C_TO_F_DIV + UNIT_TEMP_F_ZERO;
            }
        }
        return aValue;
    }

    static getTemperaturePrecision(theUnitIndx){
        if(  theUnitIndx == UNIT_TEMP_INDX_C ||  theUnitIndx == UNIT_TEMP_INDX_F ){
            return 1;
        }
        return 2;
    }


    static getPressureUnitsCount(){
        return UnitConverter._s_PressureUnits.length;
    }

    static getPressureUnit(theIndx, theLang)
    {
        if( ( theIndx >= 0 ) && ( theIndx < UnitConverter._s_PressureUnits.length ) ){
            return UnitConverter._s_PressureUnits[theIndx];
        }
        return "";
    }

    static _convertPressureFromMMHG(theValue, theIndxTo){
        if( theIndxTo == UNIT_PRESS_INDX_KPA ){
            return theValue * UNIT_MMHG_TO_KPA;
        }
        if( theIndxTo == UNIT_PRESS_INDX_PSI ){
            return theValue * UNIT_MMHG_TO_PSI;
        }
        if( theIndxTo == UNIT_PRESS_INDX_BAR ){
            return theValue * UNIT_MMHG_TO_BAR;
        }
        if( theIndxTo == UNIT_PRESS_INDX_AT){
            return theValue * UNIT_MMHG_TO_ATM;
        }
        return theValue;
    }

    static _convertPressureToMMHG(theValue, theIndxFrom){
        if( theIndxFrom == UNIT_PRESS_INDX_KPA ){
            return theValue / UNIT_MMHG_TO_KPA;
        }
        if( theIndxFrom == UNIT_PRESS_INDX_PSI ){
            return theValue / UNIT_MMHG_TO_PSI;
        }
        if( theIndxFrom == UNIT_PRESS_INDX_BAR ){
            return theValue / UNIT_MMHG_TO_BAR;
        }
        if( theIndxFrom == UNIT_PRESS_INDX_AT){
            return theValue / UNIT_MMHG_TO_ATM;
        }
        return theValue;
    }

    static convertPressure(theValue, theIndxFrom, theIndxTo)
    {
        let aValue = theValue;
        if( theIndxFrom == theIndxTo )
            return aValue;
        aValue = UnitConverter._convertPressureToMMHG(theValue, theIndxFrom);
        aValue = UnitConverter._convertPressureFromMMHG(aValue, theIndxTo);
        return aValue;
    }

    static getPressurePrecision(theUnitIndx){
        if( theUnitIndx == UNIT_PRESS_INDX_MMHG ){
            return 0;
        }
        return 2;
    }
}

export {UnitConverter};