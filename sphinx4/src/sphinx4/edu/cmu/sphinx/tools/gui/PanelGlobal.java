/*
 * PanelGlobal.java
 *
 * Created on October 25, 2006, 12:03 PM
 * 
 * Portions Copyright 2007 Mitsubishi Electric Research Laboratories.
 * Portions Copyright 2007 Harvard Extension Schoool, Harvard University
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 */

package edu.cmu.sphinx.tools.gui;

import javax.swing.table.DefaultTableModel;
import javax.swing.ListSelectionModel;
import javax.swing.JOptionPane;

import java.util.Vector;
import java.util.Map;
import java.util.HashMap;
import java.util.Iterator;

/**
 * This is for the Global Settings panel
 *
 * @author  Ariani
 */
 class PanelGlobal extends javax.swing.JPanel implements GUIFileActionListener {
    
    private String _title;
    private GUIMediator _gm;
     
    private static final int MODE_OK = 10;
    private static final int MODE_SKIP = 20;
    private static final int MODE_ERROR = 30;
    
    /**
     * Creates new form PanelGlobal, only accessible to this package
     */
    PanelGlobal(String title,GUIMediator gm) {
        _title = new String(title);
        initComponents();
        _gm = gm;
        _gm.registerPanel(this); // register as listener to the GUI Mediator
    }
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    // <editor-fold defaultstate="collapsed" desc=" Generated Code ">//GEN-BEGIN:initComponents
    private void initComponents() {
        java.awt.GridBagConstraints gridBagConstraints;

        jPanel1 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        jLabel1 = new javax.swing.JLabel();
        TextFieldPropVal = new javax.swing.JTextField();
        TextFieldPropName = new javax.swing.JTextField();
        LabelTitle = new javax.swing.JLabel();
        jLabel4 = new javax.swing.JLabel();
        jPanel2 = new javax.swing.JPanel();
        ButtonAdd = new javax.swing.JButton();
        ButtonDelete = new javax.swing.JButton();
        jScrollPane1 = new javax.swing.JScrollPane();
        TableParameter = new javax.swing.JTable();

        setLayout(new java.awt.BorderLayout(10, 20));

        jPanel1.setLayout(new java.awt.GridBagLayout());

        jPanel1.setFocusable(false);
        jLabel2.setText("Property Value");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        jPanel1.add(jLabel2, gridBagConstraints);

        jLabel1.setText("Property Name");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 1;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        jPanel1.add(jLabel1, gridBagConstraints);

        TextFieldPropVal.setMinimumSize(new java.awt.Dimension(100, 19));
        TextFieldPropVal.setPreferredSize(new java.awt.Dimension(100, 19));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 5;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        jPanel1.add(TextFieldPropVal, gridBagConstraints);

        TextFieldPropName.setMinimumSize(new java.awt.Dimension(100, 19));
        TextFieldPropName.setPreferredSize(new java.awt.Dimension(100, 19));
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 3;
        gridBagConstraints.gridy = 4;
        gridBagConstraints.insets = new java.awt.Insets(3, 3, 3, 3);
        jPanel1.add(TextFieldPropName, gridBagConstraints);

        LabelTitle.setBackground(new java.awt.Color(153, 153, 153));
        LabelTitle.setText(_title);
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 0;
        gridBagConstraints.gridwidth = 5;
        gridBagConstraints.gridheight = 2;
        gridBagConstraints.insets = new java.awt.Insets(10, 10, 10, 10);
        jPanel1.add(LabelTitle, gridBagConstraints);

        jLabel4.setText("                                             ");
        gridBagConstraints = new java.awt.GridBagConstraints();
        gridBagConstraints.gridx = 0;
        gridBagConstraints.gridy = 2;
        gridBagConstraints.gridwidth = 5;
        gridBagConstraints.gridheight = 2;
        jPanel1.add(jLabel4, gridBagConstraints);

        add(jPanel1, java.awt.BorderLayout.NORTH);

        jPanel2.setFocusable(false);
        ButtonAdd.setText("Add");
        ButtonAdd.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ButtonAddActionPerformed(evt);
            }
        });

        jPanel2.add(ButtonAdd);

        ButtonDelete.setText("Delete");
        ButtonDelete.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                ButtonDeleteActionPerformed(evt);
            }
        });

        jPanel2.add(ButtonDelete);

        add(jPanel2, java.awt.BorderLayout.SOUTH);

        TableParameter.setModel(new javax.swing.table.DefaultTableModel(
            new Object [][] {

            },
            new String [] {
                "Property Name", "Property Value"
            }
        ) {
            Class[] types = new Class [] {
                java.lang.String.class, java.lang.String.class
            };
            boolean[] canEdit = new boolean [] {
                false, false
            };

            public Class getColumnClass(int columnIndex) {
                return types [columnIndex];
            }

            public boolean isCellEditable(int rowIndex, int columnIndex) {
                return canEdit [columnIndex];
            }
        });
        jScrollPane1.setViewportView(TableParameter);

        add(jScrollPane1, java.awt.BorderLayout.CENTER);

    }// </editor-fold>//GEN-END:initComponents

    /* the delete button is pressed, should delete the row that's selected from table */
    private void ButtonDeleteActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ButtonDeleteActionPerformed
        ListSelectionModel mySelectionModel = TableParameter.getSelectionModel();
        DefaultTableModel myModel = (DefaultTableModel)TableParameter.getModel();
        
        int myIndex = mySelectionModel.getMinSelectionIndex();
        if (myIndex == -1 || myIndex == myModel.getRowCount() ){ // nothing is selected in the table
            displayError("The are nothing to be deleted");
        }
        else if (myModel.getRowCount() == 0){
            displayError("There are no data in the table");
        }
        else {
            if(myModel != null)
                removeTableData(myIndex,myModel);
        }
        
    }//GEN-LAST:event_ButtonDeleteActionPerformed

    /* the add button is pressed, should add a property value to the column */
    private void ButtonAddActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_ButtonAddActionPerformed
        String propName = TextFieldPropName.getText();
        String propVal =TextFieldPropVal.getText();
        
        if ( (propName == null) || (propName.trim().equalsIgnoreCase("")) ||
             (propVal == null) || (propVal.trim().equalsIgnoreCase("")))
        {
            //if either property name or value is empty
            displayError("Please complete the property name and value first");
        }
        else /* both property name and value are available */        
            addTableData(propName, propVal);
        
    }//GEN-LAST:event_ButtonAddActionPerformed
     
    /** remove an item from table
     */
    private void removeTableData (int index, DefaultTableModel model){
         model.removeRow(index);                
         TableParameter.revalidate();
        
    }
    
    /**
     * add a row to the table, consisting property value and property name 
     */
    private void addTableData(String propName, String propVal){
        DefaultTableModel model = (DefaultTableModel)TableParameter.getModel();
        Vector myData = new Vector(2);
        myData.add(propName);
        myData.add(propVal);
        model.insertRow(model.getRowCount(),myData);
        TableParameter.revalidate();
       
    }
    
    /**
     * delete all rows from the table
     */
    public void clearAll() {
        
        DefaultTableModel model =(DefaultTableModel)TableParameter.getModel();
        int numRows = model.getRowCount();
        
        for (int i=numRows-1;i>=0;i--) {            
           removeTableData(i,model);
        }      
    }
    
    /**
     * update the table according to the configProperties data
     */
    public void update(ConfigProperties cp) {
        Map myglobal=cp.getGlobal();
        this.clearAll(); 
        // iterate each item in the global properties Map, and display it
        for ( Iterator it = myglobal.entrySet().iterator(); it.hasNext();){
            Map.Entry entry = (Map.Entry) it.next();            
            String name = (String)entry.getKey();
            String value = (String)entry.getValue();
            addTableData(name,value);
        }        
    }

    /** save the data from table
     */
    public void saveData(ConfigProperties cp) throws GUIOperationException {
        Map globalMap;
        if (cp.getGlobal() == null)
            globalMap = new HashMap();
        else
            globalMap = cp.getGlobal();
            
        iterateTable(globalMap);
        cp.setGlobal(globalMap);

    }
    
    /** the sphinx model is being reloaded */
    public void modelRefresh() {
        // do nothing
    }
    /**
     * iterate through the Table and verify the values to make sure that they are valid
     * 
     * @throws GUIOperationException
     */
    private void iterateTable(Map globalMap)throws GUIOperationException{
        DefaultTableModel model =(DefaultTableModel)TableParameter.getModel();
        String propName;
        String propVal;
        int retval;
        
        int numRows = model.getRowCount();
        
        for (int i=0;i<numRows;i++) {            
            propName = ((String)TableParameter.getValueAt(i,0)).trim();
            propVal = ((String)TableParameter.getValueAt(i,1)).trim();
            
            System.out.print("validating " + propName + " and " + propVal);
            retval = validateProp(propName,propVal);
            switch(retval){
                case MODE_OK:
                    globalMap.put(propName,propVal);
                    break;
                case MODE_ERROR:
                    displayError("File Save error : Global property row " + (i+1) + " is invalid ");
                    throw new GUIOperationException(GUIOperationException.EXCEPTION_GLOBAL,
                            "Data from Global property table is invalid");                  
                case MODE_SKIP:
                    break;
            }
        }

    }
    
    /** check if there's a valid name and value for each global property in the table
     */
    private int validateProp(String propName, String propVal)
    {
         if( propName == null || propName.trim().equalsIgnoreCase("") )
         {
             // if the name is empty, then value must be empty too
             if(propVal == null || propVal.trim().equalsIgnoreCase("") )
                 return MODE_SKIP;
             else
                 return MODE_ERROR;
         }
         else // name is not empty, check the prop value 
         {
             // name is not empty, must check if it's a one word name
             if( propName.trim().indexOf(" ") == -1 ){
                //no space in propName, check propValue
                if( propVal!= null && propVal.trim().indexOf(" ") == -1 )
                    return MODE_OK;
                else
                    return MODE_ERROR;
             }                 
             else // there are space(s) in the property name
                 return MODE_ERROR;
         }             
                    
    }
    
    /** private helper function to display the error to user
     */
    private void displayError(String message) {
        JOptionPane.showMessageDialog(this,message, 
                        "Global Property", JOptionPane.ERROR_MESSAGE);
    }


    
    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton ButtonAdd;
    private javax.swing.JButton ButtonDelete;
    private javax.swing.JLabel LabelTitle;
    private javax.swing.JTable TableParameter;
    private javax.swing.JTextField TextFieldPropName;
    private javax.swing.JTextField TextFieldPropVal;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JPanel jPanel1;
    private javax.swing.JPanel jPanel2;
    private javax.swing.JScrollPane jScrollPane1;
    // End of variables declaration//GEN-END:variables
   

    
   
}
