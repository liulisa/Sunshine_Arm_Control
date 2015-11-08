/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.DownloadableContributionVersionComparator;
import cc.arduino.contributions.VersionComparator;
import cc.arduino.contributions.filters.BuiltInPredicate;
import cc.arduino.contributions.filters.InstalledPredicate;
import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.filters.OnlyUpstreamReleasePredicate;
import cc.arduino.contributions.ui.InstallerTableCell;
import cc.arduino.contributions.ui.listeners.DelegatingKeyListener;
import cc.arduino.utils.ReverseComparator;
import processing.app.Base;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.event.HyperlinkEvent;
import javax.swing.text.Document;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.StyleSheet;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;
import java.util.stream.Collectors;

import static processing.app.I18n.format;
import static processing.app.I18n.tr;

@SuppressWarnings("serial")
public class ContributedLibraryTableCell extends InstallerTableCell {

  private final JPanel panel;
  private final JButton installButton;
  private final Component installButtonPlaceholder;
  private JComboBox downgradeChooser;
  private final JComboBox versionToInstallChooser;
  private final JButton downgradeButton;
  private final JPanel buttonsPanel;
  private final JPanel inactiveButtonsPanel;
  private final JLabel statusLabel;

  public ContributedLibraryTableCell() {
    {
      installButton = new JButton(tr("Install"));
      installButton.addActionListener(e -> onInstall(editorValue.getSelected(), editorValue.getInstalled()));
      int width = installButton.getPreferredSize().width;
      installButtonPlaceholder = Box.createRigidArea(new Dimension(width, 1));
    }

    downgradeButton = new JButton(tr("Install"));
    downgradeButton.addActionListener(e -> {
      ContributedLibrary selected = (ContributedLibrary) downgradeChooser.getSelectedItem();
      onInstall(selected, editorValue.getInstalled());
    });

    downgradeChooser = new JComboBox();
    downgradeChooser.addItem("-");
    downgradeChooser.setMaximumSize(downgradeChooser.getPreferredSize());
    downgradeChooser.addItemListener(e -> {
      Object selectVersionItem = downgradeChooser.getItemAt(0);
      boolean disableDowngrade = (e.getItem() == selectVersionItem);
      downgradeButton.setEnabled(!disableDowngrade);
    });

    versionToInstallChooser = new JComboBox();
    versionToInstallChooser.addItem("-");
    versionToInstallChooser.setMaximumSize(versionToInstallChooser.getPreferredSize());
    versionToInstallChooser.addItemListener(e -> editorValue.select((ContributedLibrary) versionToInstallChooser.getSelectedItem()));

    panel = new JPanel();
    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

    makeNewDescription(panel);

    {
      buttonsPanel = new JPanel();
      buttonsPanel.setLayout(new BoxLayout(buttonsPanel, BoxLayout.X_AXIS));
      buttonsPanel.setOpaque(false);

      buttonsPanel.add(Box.createHorizontalStrut(7));
      buttonsPanel.add(downgradeChooser);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(downgradeButton);

      buttonsPanel.add(Box.createHorizontalGlue());

      buttonsPanel.add(versionToInstallChooser);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(installButton);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(Box.createHorizontalStrut(15));

      panel.add(buttonsPanel);
    }

    {
      inactiveButtonsPanel = new JPanel();
      inactiveButtonsPanel.setLayout(new BoxLayout(inactiveButtonsPanel, BoxLayout.X_AXIS));
      inactiveButtonsPanel.setOpaque(false);

      int height = installButton.getMinimumSize().height;
      inactiveButtonsPanel.add(Box.createVerticalStrut(height));
      inactiveButtonsPanel.add(Box.createGlue());

      statusLabel = new JLabel(" ");
      inactiveButtonsPanel.add(statusLabel);
      inactiveButtonsPanel.add(Box.createHorizontalStrut(15));

      panel.add(inactiveButtonsPanel);
    }

    panel.add(Box.createVerticalStrut(15));
  }

  private JTextPane makeNewDescription(JPanel panel) {
    if (panel.getComponentCount() > 0) {
      panel.remove(0);
    }
    JTextPane description = new JTextPane();
    description.setInheritsPopupMenu(true);
    Insets margin = description.getMargin();
    margin.bottom = 0;
    description.setMargin(margin);
    description.setContentType("text/html");
    Document doc = description.getDocument();
    if (doc instanceof HTMLDocument) {
      HTMLDocument html = (HTMLDocument) doc;
      StyleSheet stylesheet = html.getStyleSheet();
      stylesheet.addRule("body { margin: 0; padding: 0;"
        + "font-family: Verdana, Geneva, Arial, Helvetica, sans-serif;"
        + "font-size: 100%;" + "font-size: 0.95em; }");
    }
    description.setOpaque(false);
    description.setBorder(new EmptyBorder(4, 7, 7, 7));
    description.setHighlighter(null);
    description.setEditable(false);
    description.addHyperlinkListener(e -> {
      if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
        Base.openURL(e.getDescription());
      }
    });
    description.addKeyListener(new DelegatingKeyListener(parentTable));
    panel.add(description, 0);
    return description;
  }

  protected void onRemove(ContributedLibrary selected) {
    // Empty
  }

  protected void onInstall(ContributedLibrary selected, ContributedLibrary installed) {
    // Empty
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int column) {
    parentTable = table;
    setEnabled(false);
    Component component = getUpdatedCellComponent(value, isSelected, row, false);
    if (row % 2 == 0) {
      component.setBackground(new Color(236, 241, 241)); //#ecf1f1
    } else {
      component.setBackground(new Color(255, 255, 255));
    }

    int height = new Double(component.getPreferredSize().getHeight()).intValue();
    if (table.getRowHeight(row) < height) {
      table.setRowHeight(row, height);
    }

    return component;
  }

  private LibrariesIndexTableModel.ContributedLibraryReleases editorValue;
  private JTable parentTable;

  @Override
  public Object getCellEditorValue() {
    return editorValue;
  }

  @Override
  public Component getTableCellEditorComponent(JTable table, Object value,
                                               boolean isSelected, int row,
                                               int column) {
    parentTable = table;
    editorValue = (LibrariesIndexTableModel.ContributedLibraryReleases) value;
    setEnabled(true);

    final ContributedLibrary installed = editorValue.getInstalled();

    List<ContributedLibrary> releases = editorValue.releases.stream().filter(new OnlyUpstreamReleasePredicate()).collect(Collectors.toList());
    List<ContributedLibrary> uninstalledReleases = releases.stream().filter(new InstalledPredicate().negate()).collect(Collectors.toList());

    List<ContributedLibrary> installedBuiltIn = releases.stream().filter(new InstalledPredicate()).filter(new BuiltInPredicate()).collect(Collectors.toList());

    if (installed != null && !installedBuiltIn.contains(installed)) {
      uninstalledReleases.addAll(installedBuiltIn);
    }

    Collections.sort(uninstalledReleases, new ReverseComparator<>(new DownloadableContributionVersionComparator()));

    downgradeChooser.removeAllItems();
    downgradeChooser.addItem(tr("Select version"));

    final List<ContributedLibrary> uninstalledPreviousReleases = new LinkedList<>();
    final List<ContributedLibrary> uninstalledNewerReleases = new LinkedList<>();

    final VersionComparator versionComparator = new VersionComparator();
    uninstalledReleases.stream().forEach(input -> {
      if (installed == null || versionComparator.greaterThan(installed.getParsedVersion(), input.getParsedVersion())) {
        uninstalledPreviousReleases.add(input);
      } else {
        uninstalledNewerReleases.add(input);
      }
    });
    uninstalledNewerReleases.forEach(downgradeChooser::addItem);
    uninstalledPreviousReleases.forEach(downgradeChooser::addItem);

    downgradeChooser.setVisible(installed != null && (!uninstalledPreviousReleases.isEmpty() || uninstalledNewerReleases.size() > 1));
    downgradeButton.setVisible(installed != null && (!uninstalledPreviousReleases.isEmpty() || uninstalledNewerReleases.size() > 1));

    versionToInstallChooser.removeAllItems();
    uninstalledReleases.forEach(versionToInstallChooser::addItem);
    versionToInstallChooser.setVisible(installed == null && uninstalledReleases.size() > 1);

    Component component = getUpdatedCellComponent(value, true, row, !installedBuiltIn.isEmpty());
    component.setBackground(new Color(218, 227, 227)); //#dae3e3
    return component;
  }

  private Component getUpdatedCellComponent(Object value, boolean isSelected, int row, boolean hasBuiltInRelease) {
    LibrariesIndexTableModel.ContributedLibraryReleases releases = (LibrariesIndexTableModel.ContributedLibraryReleases) value;

    JTextPane description = makeNewDescription(panel);

    //FIXME: happens on macosx, don't know why
    if (releases == null) {
      return panel;
    }

    ContributedLibrary selected = releases.getSelected();
    ContributedLibrary installed = releases.getInstalled();

    boolean installable, upgradable;
    if (installed == null) {
      installable = true;
      upgradable = false;
    } else {
      installable = false;
      upgradable = new DownloadableContributionVersionComparator().compare(selected, installed) > 0;
    }
    if (installable) {
      installButton.setText(tr("Install"));
    }
    if (upgradable) {
      installButton.setText(tr("Update"));
    }
    installButton.setVisible(installable || upgradable);
    installButtonPlaceholder.setVisible(!(installable || upgradable));

    String name = selected.getName();
    String author = selected.getAuthor();
    // String maintainer = selectedLib.getMaintainer();
    String website = selected.getWebsite();
    String sentence = selected.getSentence();
    String paragraph = selected.getParagraph();
    // String availableVer = selectedLib.getVersion();
    // String url = selected.getUrl();

    String midcolor = isSelected ? "#000000" : "#888888";

    String desc = "<html><body>";

    // Library name...
    desc += format("<b>{0}</b>", name);
    if (installed != null && installed.isReadOnly()) {
      desc += " Built-In ";
    }

    // ...author...
    desc += format("<font color=\"{0}\">", midcolor);
    if (author != null && !author.isEmpty()) {
      desc += format(" by <b>{0}</b>", author);
    }

    // ...version.
    if (installed != null) {
      String installedVer = installed.getParsedVersion();
      if (installedVer == null) {
        desc += " " + tr("Version unknown");
      } else {
        desc += " " + format(tr("Version <b>{0}</b>"), installedVer);
      }
    }
    desc += "</font>";

    if (installed != null) {
      desc += " <strong><font color=\"#00979D\">INSTALLED</font></strong>";
    }

    desc += "<br/>";

    // Description
    if (sentence != null) {
      desc += format("<b>{0}</b> ", sentence);
      if (paragraph != null && !paragraph.isEmpty())
        desc += format("{0}", paragraph);
      desc += "<br />";
    }
    if (author != null && !author.isEmpty()) {
      desc += format("<a href=\"{0}\">More info</a>", website);
    }

    desc += "</body></html>";
    description.setText(desc);
    description.setBackground(Color.WHITE);

    // for modelToView to work, the text area has to be sized. It doesn't
    // matter if it's visible or not.

    // See:
    // http://stackoverflow.com/questions/3081210/how-to-set-jtextarea-to-have-height-that-matches-the-size-of-a-text-it-contains
    int width = parentTable.getBounds().width;
    setJTextPaneDimensionToFitContainedText(description, width);

    if (isSelected) {
      panel.setBackground(parentTable.getSelectionBackground());
      panel.setForeground(parentTable.getSelectionForeground());
    } else {
      panel.setBackground(parentTable.getBackground());
      panel.setForeground(parentTable.getForeground());
    }

    return panel;
  }

  private final Timer enabler = new Timer(100, new ActionListener() {
    @Override
    public void actionPerformed(ActionEvent e) {
      enable(true);
      enabler.stop();
    }
  });

  @Override
  public void setEnabled(boolean enabled) {
    enable(false);
    if (enabled) {
      enabler.start();
    } else {
      enabler.stop();
    }
    buttonsPanel.setVisible(enabled);
    inactiveButtonsPanel.setVisible(!enabled);
  }

  public void enable(boolean enabled) {
    installButton.setEnabled(enabled);
  }

  public void setStatus(String status) {
    statusLabel.setText(status);
  }

  public void invalidate() {
    panel.invalidate();
  }

}
