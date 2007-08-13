package edu.cmu.sphinx.tools.confdesigner.propedit;

import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4String;

import javax.swing.*;
import javax.swing.table.TableCellEditor;
import javax.swing.table.TableCellRenderer;

/**
 * DOCUMENT ME!
 *
 * @author Holger Brandl
 */
public class TableStringProperty extends TableProperty {

    private PropertySheet currentPS;
    private String propName;
    private S4String s4String;


    public TableStringProperty(JTable myTable, PropertySheet currentPS, String propName) {
        super(propName, myTable);
        this.currentPS = currentPS;
        this.propName = propName;
        this.s4String = s4String;

        setDisplayName(propName);

        if (currentPS.getRaw(propName) != null) {
            setValue(currentPS.getDouble(propName));
        } else {
            String defValue = s4String.defaultValue();
            String[] range = s4String.range();
            if (range.length != 0) {
                JComboBox box = new JComboBox(range);
                setValue(box);
            } else {
                if (defValue.equals(S4String.NOT_DEFINED))
                    setValue(defValue);
            }

            // set color to gray to indicate the defaultness
        }
    }


    public void setValue(Object value) {

    }


    public Object getValue() {
        return null;
    }


    public TableCellRenderer getValueRenderer() {
        return null;
    }


    public TableCellEditor getValueEditor() {
        return null;
    }
}
