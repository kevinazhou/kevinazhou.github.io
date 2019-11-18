using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

[System.Serializable]
public class MenuDivision {

    [Tooltip("Set to true if the section should be the options menu")]
    public bool isOptionsMenu = false;

    public string name;

    public TextAsset[] textAssets;
    //[Tooltip("Gameobjects within the heirarchy to divide the sections")]

    //[Header("Section Names")]
    //public string[] SectionNames;
    [Space(15)]



    [Tooltip("Float distance between blocks of text in journal")]
    public float distBetweenBlocks;// = 3.0f;
    //[Tooltip("Float distance between blocks of text in Table of Contents")]
    //public float distBetweenTOCBlocks;// = 6.0f;
    [Tooltip("Padding to the side of images (float)")]
    public float ImagePadding;
    [Tooltip("The percentage of the page width that images will be if image is longer horizontally")]
    public float ImageWidthPercent;
    [Tooltip("The percentage of the page height that images will be if image is longer vertically")]
    public float ImageHeightPercent;

    [Header("Page prefabs")]
    public GameObject[] LeftPagePrefabs;
    public GameObject[] RightPagePrefabs;

}

//saved entries that are added
[System.Serializable]
public static class SavedJournalEntries
{
    public static List<string> AddedEntries = new List<string>();
}

//Single instance of MenuManager
public class MenuManager : MonoBehaviour {

    public static MenuManager instance;

    [SerializeField]
    public MenuDivision[] Divisions;

    public GameObject BackCover;

    //[Tooltip("Prefabs for the text/image blocks")]
    [Header("Text and image prefabs")]
    [Tooltip("Title prefabs must contain \"Title\" in the name")]
    public GameObject TitlePrefab;
    public GameObject TextPrefab;
    public GameObject ImgPrefab;

    [Space(15)]
    [Tooltip("Prefab for the section tabs on the right of the menu")]
    public GameObject SectionDivider;
    [Tooltip("Prefab for the divider tabs on the top of the menu")]
    public GameObject DivisionDivider;

    [Tooltip("List of items included in the options menu")]
    //public GameObject MusicSlider;
    //public GameObject SFXSlider;
    //public GameObject UISlider;
    public List<GameObject> OptionsMenuItems = new List<GameObject>();


    public GameObject ExitButton;

    //the actual gameobjects that hold the managers
    //TODO: probably make this private later and create some getters
    public List<GameObject> DivisionManagers = new List<GameObject>();

    public List<GameObject> DivisionUI = new List<GameObject>();

    //List of tabs
    private List<GameObject> DivisionTabList = new List<GameObject>();

    public int CurrentDivisionIndex = -1;

    public GameObject LeftArrow;
    public GameObject RightArrow;

    //false until all the journal managers and UI's are done
    public bool allInitialized = false;

    //testing, delete later
    private bool notAdded = true;

    // Use this for initialization
    void Start () {
        if(instance == null)
        {
            instance = this;
        }


        for(int i = 0; i < Divisions.Length; i++)
        {
            //Creates the JournalManagers
            GameObject temp = new GameObject(Divisions[i].name);
            temp.transform.SetParent(BackCover.transform, false);
            JournalManager JM = temp.AddComponent<JournalManager>();
            JM.MenuSettings = Divisions[i];
            //DivisionObjects.Add(temp); Managers now add themselves after creation

            //Create the dividers on the top
            GameObject tab = Instantiate(DivisionDivider);
            tab.transform.SetParent(BackCover.transform, false);
            RectTransform tabRect = tab.GetComponent<RectTransform>();
            tabRect.localPosition = new Vector3(tabRect.localPosition.x + (90 * i), tabRect.localPosition.y, tabRect.localPosition.z);
            tab.transform.GetChild(0).GetComponent<Text>().text = i.ToString();
            tab.GetComponent<DivisionTabs>().SetIndexNumber(i);

            DivisionTabList.Add(tab);
        }

    }

    /// <summary>
    /// Jumps to a the first page of a certain division in the menu
    /// </summary>
    /// <param name="index">Index of the division to jump to</param>
    public void DivisionTabClick(int index)
    {
        SetMenuDivision(index);
        DivisionUI[index].GetComponent<JournalUI>().SetCurrentSection(0);
        DivisionUI[index].GetComponent<JournalUI>().SetCurrentPage(0);
    }
	
	// Update is called once per frame
	void Update () {
        if(Input.GetKey(KeyCode.A))
        {
            //Testing, delete later
            if (notAdded)
            {
                AddEntry("hidden lorem");
                notAdded = false;
            }
        }

        if(Input.GetKey(KeyCode.Q))
        {
            AddEntry("To add");
        }
    }

    /// <summary>
    /// Opens and closes the menu, returns the state of the menu
    /// </summary>
    /// <returns>Bool, state of the menu</returns>
    public bool OpenAndCloseMenu()
    {
        //pause game
        Time.timeScale = (Time.timeScale == 0.0f) ? 1.0f : 0.0f;
        

        foreach (string val in ScreenManager.instance.sliders.Keys)
        {
            ScreenManager.instance.sliders[val].value = PlayerPrefs.GetFloat(val, 1.0f);
        }

        gameObject.SetActive(!gameObject.activeSelf);

        return gameObject.activeSelf;
    }

    //TODO: add menumanager to screenmanager
    public void ExitGame()
    {
        OpenAndCloseMenu();
        ScreenManager.instance.ToggleExitUI();
    }

    /// <summary>
    /// Called when the user goes to the next page in the menu, calls the JournalUI next page function
    /// </summary>
    public void MenuNextPage()
    {
        //Turns to the next page in the current open UI, if returns true then go to next section
        if(DivisionUI[CurrentDivisionIndex].GetComponent<JournalUI>().NextPageClick())
        {
            if(CurrentDivisionIndex == DivisionUI.Count - 1)
            {
                return;
            }
            SetMenuDivision(CurrentDivisionIndex + 1);
            //So that the arrows disappear correctly
            JournalUI UI = DivisionUI[CurrentDivisionIndex].GetComponent<JournalUI>();
            UI.SetCurrentSection(0);
            UI.SetCurrentPage(0);
        }
    }

    /// <summary>
    /// Called when the user goes to the prev page in the menu, calls the JournalUI prev page function
    /// </summary>
    public void MenuPrevPage()
    {
        if (DivisionUI[CurrentDivisionIndex].GetComponent<JournalUI>().PrevPageClick())
        {
            if (CurrentDivisionIndex == 0)
            {
                return;
            }
            SetMenuDivision(CurrentDivisionIndex - 1);

            JournalUI UI = DivisionUI[CurrentDivisionIndex].GetComponent<JournalUI>();
            
            UI.SetCurrentSection(UI.SectionManager.Count - 1);
            UI.SetCurrentPage(UI.SectionManager[UI.SectionManager.Count - 1].numOfPages);
        }
    }

    public void SetMenuDivision(int index)
    {
        //Disable the divisiona and change the color of its divider tab
        if(CurrentDivisionIndex != -1)
        {
            DivisionUI[CurrentDivisionIndex].SetActive(false);
            DivisionTabList[CurrentDivisionIndex].GetComponent<Image>().color = new Color32(255, 255, 255, 255);
        }


        //Change the index
        CurrentDivisionIndex = index;

        DivisionUI[CurrentDivisionIndex].SetActive(true);
        DivisionTabList[CurrentDivisionIndex].GetComponent<Image>().color = new Color32(159, 102, 16, 240);
    }

    public void AddEntry(string entryTitle)
    {
        for(int i = 0; i < DivisionUI.Count; i++)
        {
            if(Divisions[i].isOptionsMenu)
            {
                Debug.Log("Options menu");
                continue;
            }

            JournalEntry ent = DivisionManagers[i].GetComponent<JournalManager>().FindEntryFromTitle(entryTitle);
            if (ent == null) continue;

            if (!DivisionUI[i].GetComponent<JournalUI>().AddEntry(ent))
            {
                Debug.Log("Entry not found in JournalUI");
                continue;
            }
            else
            {
                Debug.Log("Journal Entry \"" + ent.title + "\" is in journal");
                return;
            }
        }
    }

    public void LoadEntryFromSave(string savedTitle)
    {
        for(int i = 0; i< DivisionUI.Count; i++)
        {
            if(Divisions[i].isOptionsMenu) continue;

            JournalEntry savedEnt = DivisionManagers[i].GetComponent<JournalManager>().FindEntryFromTitle(savedTitle);
            if (savedEnt == null) continue;

            if (!DivisionUI[i].GetComponent<JournalUI>().LoadSavedEntry(savedEnt))
            {
                Debug.Log("Saved entry not found in JournalUI");
                continue;
            }
            else
            {
                Debug.Log("Journal Entry \"" + savedEnt.title + "\" is in journal");
                return;
            }
            
        }
    }


}
