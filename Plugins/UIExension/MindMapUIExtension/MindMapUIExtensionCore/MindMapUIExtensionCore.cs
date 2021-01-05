﻿
using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using System.Collections.Generic;
using System.Windows.Forms.VisualStyles;

using Abstractspoon.Tdl.PluginHelpers;
using Abstractspoon.Tdl.PluginHelpers.ColorUtil;

namespace MindMapUIExtension
{
	[System.ComponentModel.DesignerCategory("")]
	public class MindMapUIExtensionCore : Panel, IUIExtension
    {
        private const string FontName = "Tahoma";

        // ----------------------------------------------------------------------------

        private IntPtr m_HwndParent = IntPtr.Zero;
        private String m_TypeId, m_UiName;

        private Translator m_Trans;
        private UIExtension.TaskIcon m_TaskIcons;
        private System.Drawing.Font m_ControlsFont;

        private TdlMindMapControl m_MindMap;
		private ComboBox m_AlignmentCombo;

        // ----------------------------------------------------------------------------

        public MindMapUIExtensionCore(String typeId, String uiName, IntPtr hwndParent, Translator trans)
        {
            m_TypeId = typeId;
			m_UiName = uiName;
			m_HwndParent = hwndParent;
            m_Trans = trans;

            InitializeComponent();
        }

        // IUIExtension ------------------------------------------------------------------

        public bool SelectTask(UInt32 dwTaskID)
        {
			return m_MindMap.SetSelectedNode(dwTaskID);
        }

        public bool SelectTasks(UInt32[] pdwTaskIDs)
        {
            return false;
        }

        public bool SelectTask(String text, UIExtension.SelectTask selectTask, bool caseSensitive, bool wholeWord, bool findReplace)
        {
            return m_MindMap.SelectTask(text, selectTask, caseSensitive, wholeWord, findReplace);
        }

        public void UpdateTasks(TaskList tasks, UIExtension.UpdateType type)
        {
			m_MindMap.UpdateTasks(tasks, type);
        }

        public bool WantTaskUpdate(Task.Attribute attrib)
        {
            return m_MindMap.WantTaskUpdate(attrib);
        }

        public bool WantSortUpdate(Task.Attribute attrib)
        {
            return false;
        }

        public bool PrepareNewTask(ref Task task)
        {
            return true;
        }

        public bool ProcessMessage(IntPtr hwnd, UInt32 message, UInt32 wParam, UInt32 lParam, UInt32 time, Int32 xPos, Int32 yPos)
        {
            return false;
        }

        public bool GetLabelEditRect(ref Int32 left, ref Int32 top, ref Int32 right, ref Int32 bottom)
        {
			Rectangle labelRect = m_MindMap.GetSelectedItemLabelRect();

			if (labelRect.IsEmpty)
				return false;

			labelRect = m_MindMap.RectangleToScreen(labelRect);

			left = labelRect.Left;
			top = labelRect.Top;
			right = labelRect.Right;
			bottom = labelRect.Bottom;

            return true;
        }

        public UIExtension.HitResult HitTest(Int32 xPos, Int32 yPos)
        {
			UInt32 taskId = m_MindMap.HitTest(new Point(xPos, yPos));

			if (taskId != 0)
				return UIExtension.HitResult.Task;

			// else
            return UIExtension.HitResult.Tasklist;
        }

        public UInt32 HitTestTask(Int32 xPos, Int32 yPos)
        {
			return m_MindMap.HitTest(new Point(xPos, yPos));
        }

        public void SetUITheme(UITheme theme)
        {
			BackColor = theme.GetAppDrawingColor(UITheme.AppColor.AppBackLight);

			// Connection colour
            var color = theme.GetAppDrawingColor(UITheme.AppColor.AppLinesDark);

            // Make sure it's dark enough
            m_MindMap.ConnectionColor = DrawingColor.SetLuminance(color, 0.6f);
		}

		public void SetTaskFont(String faceName, int pointSize)
		{
			m_MindMap.SetFont(faceName, pointSize);
		}

		public void SetReadOnly(bool bReadOnly)
        {
            m_MindMap.ReadOnly = bReadOnly;
        }

        public void SavePreferences(Preferences prefs, String key)
        {
			prefs.WriteProfileInt(key, "RootAlignment", (int)m_MindMap.Alignment);
        }

        public void LoadPreferences(Preferences prefs, String key, bool appOnly)
        {
            if (!appOnly)
            {
				// private settings
				m_MindMap.Alignment = (MindMapControl.RootAlignment)prefs.GetProfileInt(key, "RootAlignment", (int)m_MindMap.Alignment);
				m_AlignmentCombo.SelectedIndex = (int)m_MindMap.Alignment;
			}

			bool taskColorIsBkgnd = prefs.GetProfileBool("Preferences", "ColorTaskBackground", false);
			m_MindMap.TaskColorIsBackground = taskColorIsBkgnd;

			bool showParentsAsFolders = prefs.GetProfileBool("Preferences", "ShowParentsAsFolders", false);
			m_MindMap.ShowParentsAsFolders = showParentsAsFolders;

            bool showDoneCheckboxes = prefs.GetProfileBool("Preferences", "AllowCheckboxAgainstTreeItem", false);
            m_MindMap.ShowCompletionCheckboxes = showDoneCheckboxes;
			
			m_MindMap.SetStrikeThruDone(prefs.GetProfileBool("Preferences", "StrikethroughDone", true));
        }

		public new Boolean Focus()
		{
			if (Focused)
				return false;

			// else
			return m_MindMap.Focus();
		}

		public new Boolean Focused
		{
			get { return m_MindMap.Focused; }
		}

		public Boolean Expand(MindMapControl.ExpandNode expand)
		{
			return m_MindMap.Expand(expand);
		}

		public Boolean CanExpand(MindMapControl.ExpandNode expand)
		{
			return m_MindMap.CanExpand(expand);
		}

		public Boolean CanMoveTask(UInt32 taskId, UInt32 destParentId, UInt32 destPrevSiblingId)
		{
			return m_MindMap.CanMoveTask(taskId, destParentId, destPrevSiblingId);
		}

		public Boolean MoveTask(UInt32 taskId, UInt32 destParentId, UInt32 destPrevSiblingId)
		{
			return m_MindMap.MoveTask(taskId, destParentId, destPrevSiblingId);
		}

		public bool GetTask(UIExtension.GetTask getTask, ref UInt32 taskID)
		{
			return m_MindMap.GetTask(getTask, ref taskID);
		}

        public Bitmap SaveToImage()
        {
            return m_MindMap.SaveToImage();
        }

        public Boolean CanSaveToImage()
        {
            return m_MindMap.CanSaveToImage();
        }
        		
        // Message handlers ---------------------------------------------------------------------

		protected override void OnGotFocus(EventArgs e)
		{
			base.OnGotFocus(e);

			m_MindMap.Focus();
		}

        // PRIVATE ------------------------------------------------------------------------------

        private void InitializeComponent()
        {
            m_TaskIcons = new UIExtension.TaskIcon(m_HwndParent);
            m_ControlsFont = new Font(FontName, 8, FontStyle.Regular);

			m_MindMap = new TdlMindMapControl(m_Trans, m_TaskIcons);
			m_MindMap.Anchor = AnchorStyles.Left | AnchorStyles.Top | AnchorStyles.Right | AnchorStyles.Bottom;
            m_MindMap.Font = m_ControlsFont;

            if (VisualStyleRenderer.IsSupported)
                m_MindMap.BorderStyle = BorderStyle.FixedSingle;
            else
                m_MindMap.BorderStyle = BorderStyle.Fixed3D;

			m_MindMap.SelectionChange += new SelectionChangeEventHandler(OnMindMapSelectionChange);
			m_MindMap.DragDropChange += new DragDropChangeEventHandler(OnMindMapDragDrop);
			m_MindMap.EditTaskLabel += new EditTaskLabelEventHandler(OnMindMapEditTaskLabel);
            m_MindMap.EditTaskIcon += new EditTaskIconEventHandler(OnMindMapEditTaskIcon);
            m_MindMap.EditTaskDone += new EditTaskCompletionEventHandler(OnMindMapEditTaskCompletion);

            this.Controls.Add(m_MindMap);

			var comboLabel = new Label();
			comboLabel.Font = m_ControlsFont;
			comboLabel.Text = m_Trans.Translate("Alignment");
			comboLabel.AutoSize = true;
			comboLabel.Location = new Point(-2, 8);

			this.Controls.Add(comboLabel);

			m_AlignmentCombo = new ComboBox();
			m_AlignmentCombo.Font = m_ControlsFont;
			m_AlignmentCombo.Width = 120;
			m_AlignmentCombo.Height = 200;
			m_AlignmentCombo.Location = new Point(comboLabel.Right + 10, 4);
			m_AlignmentCombo.DropDownStyle = ComboBoxStyle.DropDownList;

			m_AlignmentCombo.Items.Add(m_Trans.Translate("Centre"));
			m_AlignmentCombo.Items.Add(m_Trans.Translate("Left"));
			m_AlignmentCombo.Items.Add(m_Trans.Translate("Right"));

			m_AlignmentCombo.SelectedIndexChanged += new EventHandler(OnMindMapStyleChanged);

			this.Controls.Add(m_AlignmentCombo);
		}

		void OnMindMapStyleChanged(object sender, EventArgs e)
		{
			m_MindMap.Alignment = (MindMapControl.RootAlignment)m_AlignmentCombo.SelectedIndex;
		}

		Boolean OnMindMapEditTaskLabel(object sender, UInt32 taskId)
		{
			var notify = new UIExtension.ParentNotify(m_HwndParent);

			return notify.NotifyEditLabel();
		}

        Boolean OnMindMapEditTaskCompletion(object sender, UInt32 taskId, bool completed)
        {
            var notify = new UIExtension.ParentNotify(m_HwndParent);

            return notify.NotifyMod(Task.Attribute.DoneDate, 
                                    (completed ? DateTime.Now : DateTime.MinValue));
        }

        Boolean OnMindMapEditTaskIcon(object sender, UInt32 taskId)
        {
            var notify = new UIExtension.ParentNotify(m_HwndParent);

            return notify.NotifyEditIcon();
        }

		void OnMindMapSelectionChange(object sender, object itemData)
		{
			var taskItem = (itemData as MindMapTaskItem);
			var notify = new UIExtension.ParentNotify(m_HwndParent);

			notify.NotifySelChange(taskItem.ID);
		}

		Boolean OnMindMapDragDrop(object sender, MindMapDragEventArgs e)
		{
			var notify = new UIExtension.ParentNotify(m_HwndParent);

			if (e.copyItem)
				return notify.NotifyCopy(e.dragged.uniqueID, 
										 e.targetParent.uniqueID, 
										 e.afterSibling.uniqueID);

			// else
			return notify.NotifyMove(e.dragged.uniqueID,
									 e.targetParent.uniqueID,
									 e.afterSibling.uniqueID);
		}

        protected override void OnSizeChanged(EventArgs e)
        {
            base.OnSizeChanged(e);

			Rectangle rect = ClientRectangle;
			rect.Y = m_AlignmentCombo.Bottom + 6;
			rect.Height -= (m_AlignmentCombo.Bottom + 6);

            m_MindMap.Bounds = rect;

            Invalidate(true);
        }


    }

}