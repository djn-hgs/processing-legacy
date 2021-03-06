/*
 * To change this template, choose Tools | Templates
 * and open the template in the editor.
 */

/*
 * ExportForm.java
 *
 * Created on Feb 18, 2013, 6:58:01 PM
 */

package ams.tool;

import java.awt.Dialog;
import java.io.IOException;

import processing.app.SketchException;
import processing.core.PApplet;

/**
 * 
 * @author Peter Lager
 */
public class ExportDialog extends javax.swing.JDialog implements ABconstants {

	protected AppletBuildDetail ab;
	protected boolean sizeChanged = false;
	protected boolean descChanged = false;
	
	protected String htmlFile;
	
    /** Creates new form ExportForm */
    public ExportDialog() {
        super((Dialog)null, "Applet Exporter", true);
        initComponents();
    }

   /** Creates new form ExportForm */
    public ExportDialog(AppletBuildDetail appletBuild) {
        super((Dialog)null, "Applet Exporter and Signer", true);
        initComponents();
    	ab = appletBuild;
    	lblSketchName.setText(ab.getSketch().getName());
       	txfWidth.setText("" + ab.getSketchWidth());
       	txfHeight.setText("" + ab.getSketchHeight());
        txaNarrative.setText(ab.getDescription());  	
    }

    public void exportApplet(){
    	if(sizeChanged){
    		int sw = 0, sh = 0;
    		// Width
    		try {
    			sw = Integer.parseInt(txfWidth.getText());
    		}
    		catch(NumberFormatException nfe){
    		}
    		if(sw <= 0)
    			sw = PApplet.DEFAULT_WIDTH;
    		ab.setSketchWidth(sw);
    		// height
    		try {
    			sh = Integer.parseInt(txfHeight.getText());
    		}
    		catch(NumberFormatException nfe){
    		}
    		if(sh <= 0)
    			sh = PApplet.DEFAULT_HEIGHT;
    		ab.setSketchHeight(sh);
    	}
    	if(descChanged)
    		ab.setDescription(txaNarrative.getText());
    	
    	ab.setSeparateJARs(cbxSeparate.isSelected());
    	ab.setSignApplet(cbxSign.isSelected());
    	try {
			ab.exportApplet();
		} catch (SketchException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
     }
    
    
    /** This method is called from within the constructor to
     * initialize the form.
     * WARNING: Do NOT modify this code. The content of this method is
     * always regenerated by the Form Editor.
     */
    @SuppressWarnings("unchecked")
    // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
    private void initComponents() {

        grpHtmlTemplate = new javax.swing.ButtonGroup();
        pnlControls = new javax.swing.JPanel();
        pnlOtions2 = new javax.swing.JPanel();
        jLabel2 = new javax.swing.JLabel();
        cbxSeparate = new javax.swing.JCheckBox();
        cbxSign = new javax.swing.JCheckBox();
        pnlTemplates = new javax.swing.JPanel();
        rbtnTool = new javax.swing.JRadioButton();
        lblHtmlTemplates = new javax.swing.JLabel();
        rbtn151 = new javax.swing.JRadioButton();
        pnlButtons = new javax.swing.JPanel();
        btnExport = new javax.swing.JButton();
        btnCancel = new javax.swing.JButton();
        pnlNarrative = new javax.swing.JPanel();
        jScrollPane1 = new javax.swing.JScrollPane();
        txaNarrative = new javax.swing.JTextArea();
        pnlDetails = new javax.swing.JPanel();
        jLabel1 = new javax.swing.JLabel();
        jLabel5 = new javax.swing.JLabel();
        txfWidth = new javax.swing.JTextField();
        txfHeight = new javax.swing.JTextField();
        jLabel4 = new javax.swing.JLabel();
        jLabel3 = new javax.swing.JLabel();
        lblSketchName = new javax.swing.JLabel();

        setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
        setTitle("Export Applet");
        setMinimumSize(new java.awt.Dimension(450, 400));
        setName("frameExport"); // NOI18N
        setResizable(false);

        pnlControls.setBorder(javax.swing.BorderFactory.createLineBorder(new java.awt.Color(0, 0, 0)));

        jLabel2.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel2.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        jLabel2.setText("Applet Options");

        cbxSeparate.setText("Separate JARs");

        cbxSign.setText("Sign Applet");

        javax.swing.GroupLayout pnlOtions2Layout = new javax.swing.GroupLayout(pnlOtions2);
        pnlOtions2.setLayout(pnlOtions2Layout);
        pnlOtions2Layout.setHorizontalGroup(
            pnlOtions2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlOtions2Layout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addGroup(pnlOtions2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(jLabel2, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(cbxSign, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(cbxSeparate, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, 147, Short.MAX_VALUE)))
        );
        pnlOtions2Layout.setVerticalGroup(
            pnlOtions2Layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlOtions2Layout.createSequentialGroup()
                .addComponent(jLabel2, javax.swing.GroupLayout.PREFERRED_SIZE, 25, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(cbxSeparate)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(cbxSign)
                .addContainerGap(11, Short.MAX_VALUE))
        );

        grpHtmlTemplate.add(rbtnTool);
        rbtnTool.setText("Export Tool");
        rbtnTool.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                rbtnToolActionPerformed(evt);
            }
        });

        lblHtmlTemplates.setFont(new java.awt.Font("Tahoma", 1, 11)); // NOI18N
        lblHtmlTemplates.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        lblHtmlTemplates.setText("HTML Template");

        grpHtmlTemplate.add(rbtn151);
        rbtn151.setSelected(true);
        rbtn151.setText("Processing 1.5.1");
        rbtn151.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                rbtn151ActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout pnlTemplatesLayout = new javax.swing.GroupLayout(pnlTemplates);
        pnlTemplates.setLayout(pnlTemplatesLayout);
        pnlTemplatesLayout.setHorizontalGroup(
            pnlTemplatesLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlTemplatesLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(pnlTemplatesLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(rbtnTool, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE)
                    .addComponent(rbtn151, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE)
                    .addComponent(lblHtmlTemplates, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE))
                .addContainerGap())
        );
        pnlTemplatesLayout.setVerticalGroup(
            pnlTemplatesLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlTemplatesLayout.createSequentialGroup()
                .addComponent(lblHtmlTemplates, javax.swing.GroupLayout.PREFERRED_SIZE, 29, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(rbtn151)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(rbtnTool)
                .addContainerGap(45, Short.MAX_VALUE))
        );

        btnExport.setText("Export");
        btnExport.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnExportActionPerformed(evt);
            }
        });

        btnCancel.setText("Cancel");
        btnCancel.addActionListener(new java.awt.event.ActionListener() {
            public void actionPerformed(java.awt.event.ActionEvent evt) {
                btnCancelActionPerformed(evt);
            }
        });

        javax.swing.GroupLayout pnlButtonsLayout = new javax.swing.GroupLayout(pnlButtons);
        pnlButtons.setLayout(pnlButtonsLayout);
        pnlButtonsLayout.setHorizontalGroup(
            pnlButtonsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlButtonsLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(pnlButtonsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(btnCancel, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE)
                    .addComponent(btnExport, javax.swing.GroupLayout.Alignment.TRAILING, javax.swing.GroupLayout.DEFAULT_SIZE, 152, Short.MAX_VALUE))
                .addContainerGap())
        );
        pnlButtonsLayout.setVerticalGroup(
            pnlButtonsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, pnlButtonsLayout.createSequentialGroup()
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                .addComponent(btnExport)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                .addComponent(btnCancel))
        );

        javax.swing.GroupLayout pnlControlsLayout = new javax.swing.GroupLayout(pnlControls);
        pnlControls.setLayout(pnlControlsLayout);
        pnlControlsLayout.setHorizontalGroup(
            pnlControlsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlControlsLayout.createSequentialGroup()
                .addContainerGap()
                .addGroup(pnlControlsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(pnlOtions2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(pnlControlsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                        .addComponent(pnlButtons, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                        .addComponent(pnlTemplates, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        pnlControlsLayout.setVerticalGroup(
            pnlControlsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, pnlControlsLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(pnlTemplates, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 15, Short.MAX_VALUE)
                .addComponent(pnlOtions2, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(18, 18, 18)
                .addComponent(pnlButtons, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addContainerGap())
        );

        txaNarrative.setColumns(20);
        txaNarrative.setFont(new java.awt.Font("Arial", 0, 12));
        txaNarrative.setRows(5);
        txaNarrative.setAutoscrolls(false);
        txaNarrative.setMaximumSize(new java.awt.Dimension(600, 400));
        txaNarrative.setMinimumSize(new java.awt.Dimension(600, 400));
        txaNarrative.setPreferredSize(new java.awt.Dimension(600, 400));
        txaNarrative.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyTyped(java.awt.event.KeyEvent evt) {
                txaNarrativeKeyTyped(evt);
            }
        });
        jScrollPane1.setViewportView(txaNarrative);

        javax.swing.GroupLayout pnlNarrativeLayout = new javax.swing.GroupLayout(pnlNarrative);
        pnlNarrative.setLayout(pnlNarrativeLayout);
        pnlNarrativeLayout.setHorizontalGroup(
            pnlNarrativeLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 445, Short.MAX_VALUE)
            .addGroup(pnlNarrativeLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 445, Short.MAX_VALUE))
        );
        pnlNarrativeLayout.setVerticalGroup(
            pnlNarrativeLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGap(0, 342, Short.MAX_VALUE)
            .addGroup(pnlNarrativeLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(jScrollPane1, javax.swing.GroupLayout.DEFAULT_SIZE, 342, Short.MAX_VALUE))
        );

        jLabel1.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel1.setText("Sketch narrative");

        jLabel5.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel5.setText("Size");

        txfWidth.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txfWidthKeyPressed(evt);
            }
        });

        txfHeight.addKeyListener(new java.awt.event.KeyAdapter() {
            public void keyPressed(java.awt.event.KeyEvent evt) {
                txfHeightKeyPressed(evt);
            }
        });

        jLabel4.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel4.setText(" x ");

        jLabel3.setFont(new java.awt.Font("Tahoma", 1, 11));
        jLabel3.setText("pixels");

        lblSketchName.setBackground(new java.awt.Color(51, 51, 51));
        lblSketchName.setFont(new java.awt.Font("Tahoma", 1, 14));
        lblSketchName.setForeground(new java.awt.Color(255, 255, 255));
        lblSketchName.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
        lblSketchName.setText("Sketch Name");
        lblSketchName.setOpaque(true);

        javax.swing.GroupLayout pnlDetailsLayout = new javax.swing.GroupLayout(pnlDetails);
        pnlDetails.setLayout(pnlDetailsLayout);
        pnlDetailsLayout.setHorizontalGroup(
            pnlDetailsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(pnlDetailsLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(jLabel1, javax.swing.GroupLayout.PREFERRED_SIZE, 138, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED, 285, Short.MAX_VALUE)
                .addComponent(jLabel5, javax.swing.GroupLayout.PREFERRED_SIZE, 41, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(txfWidth, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(jLabel4)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addComponent(txfHeight, javax.swing.GroupLayout.PREFERRED_SIZE, 50, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addGap(8, 8, 8)
                .addComponent(jLabel3)
                .addContainerGap())
            .addComponent(lblSketchName, javax.swing.GroupLayout.DEFAULT_SIZE, 650, Short.MAX_VALUE)
        );
        pnlDetailsLayout.setVerticalGroup(
            pnlDetailsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, pnlDetailsLayout.createSequentialGroup()
                .addContainerGap()
                .addComponent(lblSketchName)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(pnlDetailsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING)
                    .addComponent(jLabel1)
                    .addGroup(pnlDetailsLayout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
                        .addComponent(jLabel5)
                        .addComponent(txfWidth, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(jLabel4)
                        .addComponent(txfHeight, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addComponent(jLabel3)))
                .addContainerGap())
        );

        javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
        getContentPane().setLayout(layout);
        layout.setHorizontalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.TRAILING, false)
                    .addComponent(pnlDetails, javax.swing.GroupLayout.Alignment.LEADING, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addGroup(javax.swing.GroupLayout.Alignment.LEADING, layout.createSequentialGroup()
                        .addContainerGap()
                        .addComponent(pnlNarrative, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                        .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.UNRELATED)
                        .addComponent(pnlControls, javax.swing.GroupLayout.PREFERRED_SIZE, 185, javax.swing.GroupLayout.PREFERRED_SIZE)))
                .addContainerGap(javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
        );
        layout.setVerticalGroup(
            layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
                .addComponent(pnlDetails, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(pnlControls, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                    .addComponent(pnlNarrative, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                .addContainerGap())
        );

        pack();
    }// </editor-fold>//GEN-END:initComponents

    private void rbtn151ActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_rbtn151ActionPerformed
        ab.setTemplateToUse(TEMPLATE_151);
    }//GEN-LAST:event_rbtn151ActionPerformed

    private void rbtnToolActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_rbtnToolActionPerformed
        ab.setTemplateToUse(TEMPLATE_TOOL);
   }//GEN-LAST:event_rbtnToolActionPerformed

    private void txaNarrativeKeyTyped(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txaNarrativeKeyTyped
        descChanged = true;
    }//GEN-LAST:event_txaNarrativeKeyTyped

    private void txfHeightKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txfHeightKeyPressed
        sizeChanged = true;
    }//GEN-LAST:event_txfHeightKeyPressed

    private void txfWidthKeyPressed(java.awt.event.KeyEvent evt) {//GEN-FIRST:event_txfWidthKeyPressed
        sizeChanged = true;
    }//GEN-LAST:event_txfWidthKeyPressed

    private void btnCancelActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnCancelActionPerformed
        dispose();
    }//GEN-LAST:event_btnCancelActionPerformed

    private void btnExportActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_btnExportActionPerformed
        exportApplet();
        dispose();
    }//GEN-LAST:event_btnExportActionPerformed

    /**
    * @param args the command line arguments
    */
    public static void main(String args[]) {
        java.awt.EventQueue.invokeLater(new Runnable() {
            public void run() {
                new ExportDialog().setVisible(true);
            }
        });
    }

    // Variables declaration - do not modify//GEN-BEGIN:variables
    private javax.swing.JButton btnCancel;
    private javax.swing.JButton btnExport;
    private javax.swing.JCheckBox cbxSeparate;
    private javax.swing.JCheckBox cbxSign;
    private javax.swing.ButtonGroup grpHtmlTemplate;
    private javax.swing.JLabel jLabel1;
    private javax.swing.JLabel jLabel2;
    private javax.swing.JLabel jLabel3;
    private javax.swing.JLabel jLabel4;
    private javax.swing.JLabel jLabel5;
    private javax.swing.JScrollPane jScrollPane1;
    private javax.swing.JLabel lblHtmlTemplates;
    private javax.swing.JLabel lblSketchName;
    private javax.swing.JPanel pnlButtons;
    private javax.swing.JPanel pnlControls;
    private javax.swing.JPanel pnlDetails;
    private javax.swing.JPanel pnlNarrative;
    private javax.swing.JPanel pnlOtions2;
    private javax.swing.JPanel pnlTemplates;
    private javax.swing.JRadioButton rbtn151;
    private javax.swing.JRadioButton rbtnTool;
    private javax.swing.JTextArea txaNarrative;
    private javax.swing.JTextField txfHeight;
    private javax.swing.JTextField txfWidth;
    // End of variables declaration//GEN-END:variables

}
