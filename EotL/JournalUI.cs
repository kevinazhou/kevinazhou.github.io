using System.Collections.Generic;
using TMPro;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

//CODE BY: Kevin Zhou
//NOTE: Save Journal txt files with UTF-8 encoding. Unity doesn't like anything else

#region classes and stuff
/// <summary>
/// Keeps track of the the actual UI gameobject blocks that make up this entry
/// </summary>
public class EntryManager
{
    public JournalEntry entry;
    public List<BlockTracker> blocks = new List<BlockTracker>();//the blocks that make up the entry
    public bool isPlaced = false;//whether or not it has been placed on the journal

    public EntryManager() { }


    public EntryManager(EntryManager toClone)
    {
        entry = toClone.entry;
        for(int i = 0; i < toClone.blocks.Count; i++)
        {
            if(toClone.blocks[i].GetType() == typeof(ImageBlockTracker))
            {
                ImageBlockTracker converted = (ImageBlockTracker)toClone.blocks[i];
                ImageBlockTracker imageBlockToAdd = GameObject.Instantiate(converted);
                imageBlockToAdd.SetPartner(GameObject.Instantiate(converted.PartnerBlock));
                blocks.Add(imageBlockToAdd);
                continue;
            }
            BlockTracker toAdd = GameObject.Instantiate(toClone.blocks[i]);

            blocks.Add(toAdd);
        }
        isPlaced = toClone.isPlaced;
    }
}

/// <summary>
/// Keeps track of the how filled up the page is and if it is full
/// </summary>
public class PageManager
{
    public GameObject page;
    public float heightFromTop = 0.0f;//how far from top of page that next block should be placed at
    public bool filled = false;//whether or not the page can fit more stuff, true when heightfromtop is maxed out
}

/// <summary>
/// Keeps track of the pages and stuff for each section of the journal
/// </summary>
public class SectionTracker
{
    public List<PageManager> LeftPages;
    public List<PageManager> RightPages;
    public GameObject holder;//Holder that divides the sections up in the heirarchy
    public GameObject tabDivider;//section divider that is the section tab

    public bool firstPlaced = false;//if the first entry in that section has been placed yet
    public bool OnLeftPage = true;//to keep track of what type the last page created is
    public PageManager LatestPage;//last created page
    public int currPrefab = 0;//which prefab we are currently using
    public float StartingY = 0.0f;//where the blocks should start
    public bool StartYSet = false;

    public int numOfPages = -1;//the number of pages. A page is a left and right page combined

    public int section;

    public SectionTracker(int sec, GameObject hold, GameObject div)
    {
        section = sec;
        holder = hold;
        tabDivider = div;
        LeftPages = new List<PageManager>();
        RightPages = new List<PageManager>();
    }
}
#endregion

//BEFORE: Filling takes ~.7 sec, creating ~8, Total ~9 sec
//NOW: Total ~3 seconds
//NOW: Total ~1 second

/// <summary>
/// JournalManager parses and stores all possible journal entries.
/// This class converts and stores them as EntryManagers.
/// The entries are then placed on the journal, and then the TableOfContents is updated 
/// based on what is placed on the journal
/// </summary>
public class JournalUI : MonoBehaviour
{

    #region enums
    /// <summary>
    /// Journal sections
    /// </summary>
    /*
    public enum Section
    {
        TableOfContents = 0,
        PlayerSection = 1,
        ParentSection = 2,
        EnvironmentSection = 3,
        DreamSection = 4
    }
    */

    /// <summary>
    /// Returned by CanPlaceEntryOnPage
    /// </summary>
    private enum CanPlaceStatus
    {
        PageAlreadyFilled,
        CanBePlaced,
        NextPageOccupied,
        NextPageDoesntExist
    }
    #endregion

    #region prefabs and gameobjects, define in inspector
    public JournalManager journal;//define in inspector

    public MenuDivision Settings;

    public List<GameObject> SectionHolders = new List<GameObject>();

    [Tooltip("Holds journal entries not yet discovered")]
    public GameObject Holder;//to hold blocks that are currently hidden in the heirarchy

    #endregion


    public List<SectionTracker> SectionManager = new List<SectionTracker>();

    #region private
    //keeps track of which indices correspond to which entry in the block lists
    private List<List<EntryManager>> Managers = new List<List<EntryManager>>();


    private float pageTextMaxWidth;
    private float pageTextMaxHeight;//the max dimensions of the text on the page, so it doesn't go off page

    private float minTextHeight;//the smallest height of a text box, used for wrapping text

    private int CurrentSection = 0;//the current section we are in
    private int CurrentPages = 0;//which pages we currently are on
    #endregion

    //TODO LOOK AT THIS
    public void SetCurrentSection(int num)
    {
        CurrentSection = num;
    }

    #region functions
    // Use this for initialization
    void Start()
    {
        float t = Time.realtimeSinceStartup;

        DontDestroyOnLoad(gameObject);

        Holder = new GameObject("Holder");
        Holder.transform.SetParent(this.transform, false);
        Holder.SetActive(false);

        GameObject page = Instantiate(Settings.LeftPagePrefabs[0]);
        pageTextMaxHeight = page.GetComponent<RectTransform>().rect.height * (0.7f);
        pageTextMaxWidth = page.GetComponent<RectTransform>().rect.width * (0.395f);
        Destroy(page);

        BlockTracker e = TextBoxCreate(MenuManager.instance.TextPrefab, "e", null);
        e.block.transform.SetParent(Holder.transform, false);
        RectTransform rt = e.block.GetComponent<RectTransform>();
        minTextHeight = rt.rect.height;
        minTextHeight -= (minTextHeight * 0.15f);
        //maxX = rt.localPosition.x + rt.rect.width;

        Destroy(e.block);

        for(int i = 0; i < journal.AllEntries.Count; i++)
        {
            //first create the holder gameobjects
            GameObject temp = new GameObject("Section" + i);
            temp.transform.SetParent(this.transform, false);
            SectionHolders.Add(temp);

            //now create the tabs on the right side
            GameObject tab = Instantiate(MenuManager.instance.SectionDivider);
            tab.transform.SetParent(this.transform, false);
            RectTransform tabRect = tab.GetComponent<RectTransform>();
            tabRect.localPosition = new Vector3(tabRect.localPosition.x, tabRect.localPosition.y - (25 * i), tabRect.localPosition.z);
            tab.transform.GetChild(0).GetComponent<Text>().text = i.ToString();
            tab.GetComponent<SectionTab>().SetSectionNumber(i);
            tab.GetComponent<SectionTab>().UI = this;


            //create new entrymanagers and sectiontrackers
            Managers.Add(new List<EntryManager>());
            SectionManager.Add(new SectionTracker(i, temp, tab));

            FillAtStart(journal.AllEntries[i], Managers[i]);
        }

        //TIME STUFF
        float beforeFill = Time.realtimeSinceStartup;


        float fill = Time.realtimeSinceStartup;
        float timeFillTook = fill - beforeFill;
        Debug.Log("Journal managers took " + timeFillTook + " to fill.");
        //Create the UI for every section
        foreach (SectionTracker sectrac in SectionManager)
        {
            CreateUI(sectrac);
        }


        float create = Time.realtimeSinceStartup;
        float timeCreateTook = create - fill;
        Debug.Log("Creating took " + timeCreateTook + " to generate.");

        SetCurrentPage(CurrentPages);

        //Adding to the MenuManager's list of UI objects
        MenuManager.instance.DivisionUI.Add(this.gameObject);

        //TODO: Temporary code, change later
        if (MenuManager.instance.DivisionUI.Count == 1)
        {
            MenuManager.instance.SetMenuDivision(0);
        }
        else
        {
            this.gameObject.SetActive(false);
        }

        float t2 = Time.realtimeSinceStartup;
        float timeJournalTook = t2 - t;
        Debug.Log("JournalUI took " + timeJournalTook + " to generate.");

        //Temporary code to turn off the menu after initialization
        if(MenuManager.instance.DivisionUI.Count == MenuManager.instance.Divisions.Length)
        {
            MenuManager.instance.allInitialized = true;
            MenuManager.instance.gameObject.SetActive(false);
        }
    }

    // Update is called once per frame
    void Update()
    {

    }

    /// DEPRECATED, use MenuManager.OpenAndCloseMenu()
    /// <summary>Opens journal if journal is closed, closes if journal is open. Returns state after opening or closing</summary>
    /// <returns>True if open, false if closed</returns>
    public bool OpenAndCloseJournal()
    {
        gameObject.SetActive(!gameObject.activeSelf);
        return gameObject.activeSelf;
    }

    #region nextPagePrevPageFunctions
    /// <summary>Flips to the next page</summary>
    /// <returns>True if turning to the previous menu division</returns>
    public bool NextPageClick()
    {
        //returns true if on the very last page of the journal
        if (CurrentSection == SectionManager[SectionManager.Count - 1].section && 
            CurrentPages >= GetLeftPagesFromEnum(SectionManager[SectionManager.Count - 1].section).Count - 1 ) return true;
        
        CurrentPages++;
        SetCurrentPage(CurrentPages);

        return false;
    }
    /// <summary>Flips to the previous page</summary>
    /// <returns>True if turning to the next menu division</returns>
    public bool PrevPageClick()
    {
        if (CurrentSection == SectionManager[0].section && CurrentPages == 0) return true;

        CurrentPages--;
        SetCurrentPage(CurrentPages);

        return false;
    }
    /// <summary>Returns a list of the left pages in the current section</summary>
    /// <param name="sec">Section to get left pages from</param>
    /// <returns>List of PageManager of left pages</returns>
    private List<PageManager> GetLeftPagesFromEnum(int sec)
    {
        return SectionManager[sec].LeftPages;
    }
    /// <summary>Returns a list of the right pages in the current section</summary>
    /// <param name="sec">Section to get right pages from</param>
    /// <returns>List of PageManager of right pages</returns>
    private List<PageManager> GetRightPagesFromEnum(int sec)
    {
        return SectionManager[sec].RightPages;
    }
    #endregion


    /// <summary>
    /// Displays the correct pages in the current section based on passed in int parameter
    /// </summary>
    /// <param name="curr">Page number. 0 is first 2 pages, 1 is 3rd and 4th pages, etc.</param>
    public void SetCurrentPage(int curr)
    {//all the right pages behind the current page (including current) should be active, else false, all the text on right pages always inactive unless it is the current page
     //all left pages before the current page should be active, else inactive, all the text for left pages inactive except current page   

        //Set the current section active, all others inactive
        //Also set the section divider color
        for(int i = 0; i < SectionManager.Count; i++)
        {
            if(i == (int)CurrentSection)
            {
                SectionManager[(int)CurrentSection].holder.SetActive(true);
                if(this.GetType() != typeof(OptionsUIScript))
                {
                    SectionManager[(int)CurrentSection].tabDivider.GetComponent<Image>().color =
    new Color32(159, 102, 16, 240);
                }
            }
            else
            {
                SectionManager[i].holder.SetActive(false);
                if (this.GetType() != typeof(OptionsUIScript))
                {
                    SectionManager[i].tabDivider.GetComponent<Image>().color = new Color32(255, 255, 255, 255);
                }
            }
        }

        

        List<PageManager> actualLeftPages = GetLeftPagesFromEnum(CurrentSection);
        List<PageManager> actualRightPages = GetRightPagesFromEnum(CurrentSection); 

        //flip to previous section, set page to last page on previous section
        if(curr == -1)
        {//go to previous section
            //turn off current section
            SectionManager[(int)CurrentSection].holder.SetActive(false);
            CurrentSection--;
            List<PageManager> leftpages = GetLeftPagesFromEnum(CurrentSection);

            SetCurrentPage(leftpages.Count - 1);

            return;
        }
        if(curr >= actualLeftPages.Count)
        {//Go to next section
            SectionManager[(int)CurrentSection].holder.SetActive(false);
            CurrentSection++;
            SetCurrentPage(0);
            return;
        }
        
        //Left pages before current are active, so is the current
        for (int i = 0; i < actualLeftPages.Count; i++)
        {
            if (i <= curr)
            {
                actualLeftPages[i].page.SetActive(true);
            }
            else
            {
                actualLeftPages[i].page.SetActive(false);
            }
            SetChildrenActiveState(actualLeftPages[i].page, false);
        }

        //Right pages before current are inactive
        for (int i = 0; i < actualRightPages.Count; i++)
        {
            if (i < curr)
            {
                actualRightPages[i].page.SetActive(false);
            }
            else
            {
                actualRightPages[i].page.SetActive(true);
            }
            SetChildrenActiveState(actualRightPages[i].page, false);
        }
      
        if (actualRightPages.Count != curr)
        {
            SetChildrenActiveState(actualRightPages[curr].page, true);
        }

        SetChildrenActiveState(actualLeftPages[curr].page, true);
        CurrentPages = curr;

        //hide buttons if on first or last page
        if (MenuManager.instance.CurrentDivisionIndex == 0)
        {
            if (CurrentSection == SectionManager[0].section && CurrentPages == 0)
            {
                MenuManager.instance.LeftArrow.SetActive(false);
            }
            else MenuManager.instance.LeftArrow.SetActive(true);
        }
        else
        {
            MenuManager.instance.LeftArrow.SetActive(true);
        }

        if (MenuManager.instance.CurrentDivisionIndex == MenuManager.instance.Divisions.Length - 1)
        {
            if (CurrentSection == SectionManager[SectionManager.Count - 1].section &&
        CurrentPages >= GetLeftPagesFromEnum(SectionManager[SectionManager.Count - 1].section).Count - 1)
            {
                MenuManager.instance.RightArrow.SetActive(false);
            }
            else MenuManager.instance.RightArrow.SetActive(true);
        }

        else
        {
            MenuManager.instance.RightArrow.SetActive(true);
        }


        //TOCButton.SetActive(CurrentSection != Section.TableOfContents);
        
    }

    /// <summary>
    /// Creates the journal UI. Call once at the beginning of the game or if you need to redo the journal.
    /// </summary>
    /// <param name="section">The section to generate</param>
    private void CreateUI(SectionTracker section)
    {
        foreach(Transform child in section.holder.transform)
        {
            Destroy(child.gameObject);
        }

        section.LeftPages.Clear();
        section.RightPages.Clear();

        section.LatestPage = new PageManager();

        for (int z = 0; z < 3; z++)
        {//creating the right pages, to show that there are pages left to write on

            GameObject temp;

            temp = Instantiate(Settings.RightPagePrefabs[z]);
            //TEST, changing to true
            temp.transform.SetParent(section.holder.transform, false);

        }

        section.LatestPage.page = Instantiate(Settings.LeftPagePrefabs[section.currPrefab % 3]);

        section.LatestPage.page.transform.SetParent(section.holder.transform, false);
        section.LeftPages.Add(section.LatestPage);
        //page.SetActive(false);

        List<EntryManager> GetMeYourManager = Managers[(int)section.section];

        List<EntryManager> SpecialEntries = new List<EntryManager>();

        //first place the entries that start on a specific page
        //if(SpecialEntries.Count > 0) PlaceEntriesInJournal(SpecialEntries, section);
        //Debug.Log("NOW PLACING NORMAL ONES");
        
        PlaceEntriesInJournal(GetMeYourManager, section);


        //SetCurrentPage(CurrentPages);
    }

    /// <summary>
    /// Adds ans entry to the journal. Places entry in the first location where it can fit.
    /// If entry has a specified start page, it will place it in the first location where it can fit on or after the start page.
    /// </summary>
    /// <param name="entry">JournalEntry to add</param>
    /// <returns>true if entry is successfully placed, false if no entry is found and placed</returns>
    public bool AddEntry(JournalEntry entry)
    {
        if(entry == null)
        {
            Debug.Log("Error: Journal Entry provided is null.");
            return false;
        }
        for (int sec = 0; sec < Managers.Count; sec++)
        {
            foreach(EntryManager man in Managers[sec])
            {
                if(man.entry.Equals(entry))
                {
                    if(man.isPlaced)
                    {
                        Debug.Log("Journal Entry " + entry.title + " has already been added and placed");
                        return true;
                    }

                    entry.display = true;
                    List<EntryManager> single = new List<EntryManager>();
                    single.Add(man);
                    PlaceEntriesInJournal(single, SectionManager[sec]);

                    SetCurrentPage(CurrentPages);
                    SavedJournalEntries.AddedEntries.Add(entry.title);
                    Debug.Log("Journal Entry " + entry.title + " saved");

                    //JournalButtonScript.instance.TurnOnAndOffGlow();

                    return true;
                }
            }
        }
        Debug.Log("Journal Entry \"" + entry.title + "\" could not be found.");
        return false;
    }

    /// Loads saved entries from save file, to restore journal to saved state.
    /// </summary>
    /// <param name="entry">JournalEntry to load</param>
    /// <returns>true if entry successfully loaded, false if no entry found and loaded</returns>
    public bool LoadSavedEntry(JournalEntry entry)
    {
        if (entry == null)
        {
            Debug.Log("Error: Saved Journal Entry provided is null.");
            return false;
        }
        for (int sec = 0; sec < Managers.Count; sec++)
        {
            foreach (EntryManager man in Managers[sec])
            {
                if (man.entry.Equals(entry))
                {
                    if (man.isPlaced)
                    {
                        Debug.Log("Journal Entry " + entry.title + " has already been added and placed");
                        return true;
                    }

                    entry.display = true;
                    List<EntryManager> single = new List<EntryManager>();
                    single.Add(man);
                    PlaceEntriesInJournal(single, SectionManager[sec]);

                    SetCurrentPage(CurrentPages);                    
                    return true;
                }
            }
        }
        Debug.Log("Saved Journal Entry \"" + entry.title + "\" could not be found.");
        return false;
    }

    /// <summary>
    /// Places all the entries in a list of EntryManager into the journal. 
    /// Finds where the entry can fit using CanPlaceEntryOnPage before calling SetBlocksOnPage to place the entry.
    /// Always first places entries with specified starting page first.
    /// </summary>
    /// <param name="entries">List of EntryManager containing entries to place</param>
    /// <param name="section">The section to place the entries in</param>
    private void PlaceEntriesInJournal(List<EntryManager> entries, SectionTracker section)
    {
        bool StartPageEntriesPlaced = false;
        for (int twoPasses = 0; twoPasses < 2; twoPasses++)
        {//first pass places the start page ones, then places the other ones            
            for (int yeet = 0; yeet < entries.Count; yeet++)
            {
                //float start = Time.realtimeSinceStartup;

                EntryManager entry = entries[yeet];
                if (entry.isPlaced) continue;//skip if the entry has already been placed in the journal

                if (!entry.entry.display) continue;//skip the entries that should not be placed at start            

                //set where the starty position is for the section
                if (!section.StartYSet)
                {
                    RectTransform r = entry.blocks[0].block.GetComponent<RectTransform>();
                    section.StartingY = r.localPosition.y + r.rect.height / 2;
                    section.StartYSet = true;
                }

                //first check if it is a special entry that should start on a specific page
                int startPage = entry.entry.startOnPage;
                if (startPage != -1 && !StartPageEntriesPlaced)
                {//if it should start on a specific page

                    //now, we see if the current section already has enough pages generated
                    int currNumPages = section.LeftPages.Count + section.RightPages.Count;

                    while (currNumPages < startPage + 1)
                    {//if there are not enough, create pages until there are enough
                        CreateBlankPage(section);
                        currNumPages = section.LeftPages.Count + section.RightPages.Count;
                    }

                    //now, find the first page not filled at or behind the startpage
                    int leftIfZero = startPage % 2;//if left is 0, then should start on a left page
                    int index = startPage / 2;//what index the page should be

                    //Now, we check if the entire entry will either fit on the page, or will wrap around to a page
                    //that has nothing on it (HeightFromTop of 0.0f)
                    //if the page is filled, just keep finding a page until we get one that is not filled
                    bool left = leftIfZero == 0 ? true : false;

                    PageManager pageToWriteTo = PlaceEntriesHelper(ref entry, left, index, section);

                    entries[yeet] = entry;//have to set the reference in the list to the new entry

                    SetBlocksOnPage(entry.blocks[0], entry.blocks[entry.blocks.Count - 1], entry.blocks, section, pageToWriteTo);
                    entry.isPlaced = true;
                }
                else if(StartPageEntriesPlaced)
                {
                    //find the first place where we can place this entry and place it there
                    bool left = true;
                    int index = 0;

                    PageManager pageToWriteTo = PlaceEntriesHelper(ref entry, left, index, section);

                    entries[yeet] = entry;

                    SetBlocksOnPage(entry.blocks[0], entry.blocks[entry.blocks.Count - 1], entry.blocks, section, pageToWriteTo);
                    entry.isPlaced = true;

                }


                //float end = Time.realtimeSinceStartup;
                //float tot = end - start;
                //Debug.Log(entry.entry.title + " TOOK " + tot);

            }
            if (!StartPageEntriesPlaced) StartPageEntriesPlaced = true;


        }
        
    }

    /// <summary>
    /// Returns the first page behind the starting index that can fit the entire entry.
    /// </summary>
    /// <param name="entry">Entry to place</param>
    /// <param name="left">True if starting page is a left page</param>
    /// <param name="index">Index of the starting page</param>
    /// <param name="section">Section in use</param>
    /// <returns>PageManager of first page that can fit the entry</returns>
    private PageManager PlaceEntriesHelper(ref EntryManager entry, bool left, int index, SectionTracker section)
    {
        //float testTime = Time.realtimeSinceStartup;

        PageManager pageToWriteTo = left ? section.LeftPages[index] : section.RightPages[index];
        CanPlaceStatus canPlace = CanPlaceEntryOnPage(entry, section, pageToWriteTo);
        while (canPlace != CanPlaceStatus.CanBePlaced)
        {
            if (canPlace == CanPlaceStatus.PageAlreadyFilled)
            {
                if (GetNextPage(section, pageToWriteTo) == null)
                {
                    CreateBlankPage(section);
                }

            }
            else if (canPlace == CanPlaceStatus.NextPageDoesntExist)
            {//entry will wrap to next page but there is no next page in the section
                CreateBlankPage(section);

                canPlace = CanPlaceEntryOnPage(entry, section, pageToWriteTo);
                if (canPlace == CanPlaceStatus.CanBePlaced)
                {
                    break;
                }
                continue;
            }

            pageToWriteTo = left ? section.RightPages[index] : section.LeftPages[++index];

            left = !left;
            canPlace = CanPlaceEntryOnPage(entry, section, pageToWriteTo);
        }//pageToWriteTo is the page that we can write to once the while loop breaks

        
        bool hasImage = false;
        foreach(BlockTracker blok in entry.blocks)
        {
            if (blok.GetType() == typeof(ImageBlockTracker)) hasImage = true;
        }

        if (hasImage)
        {//now we check if the images in the entry will wrap, in which case we push the image up
            
            JournalEntry OldEntry = new JournalEntry(entry.entry);
            //EntryManager oneEntry = null;           

            int b = DoesImageConflict(entry, section, pageToWriteTo);
            //b contains the number of the image which conflicts
            EntryManager ManagerToAlter = null;

            if(b != -1)
            {//only create a clone if we have to speed up loadtime
                ManagerToAlter = new EntryManager(entry); 
            }           
            while (b != -1)
            {//while images do conflict, keep rearranging them

                //if(oneEntry != null)
                //{
                //    DestroyEntryManager(ref oneEntry);            
                //}
                JournalEntry AlteredEntry = new JournalEntry(OldEntry);
                int ImagesPassed = 0;

                for(int rip = 0; rip < AlteredEntry.textsAndImages.Count; rip++)
                {//altering the entry
                    if (AlteredEntry.textsAndImages[rip].image != null)
                    {//found an image block
                        if(ImagesPassed == b)
                        {//change this one
                            //already at the top of the current block, cannot push any higher
                            if(AlteredEntry.textsAndImages[rip].image.index == 0)
                            {//move to next page
                                DestroyEntryManager(ref ManagerToAlter);

                                if (pageToWriteTo.heightFromTop == 0.0f)
                                {
                                    //float testEndTime1 = Time.realtimeSinceStartup;
                                    //float toDis1 = testEndTime1 - testTime;

                                    //Debug.Log("PREMATURE RETURN: " + entry.entry.title + " TOOK " + toDis1);
                                    return pageToWriteTo;
                                }
                                pageToWriteTo = GetNextPage(section, pageToWriteTo);

                                bool left2 = false;
                                int index2 = 0;
                                //find which is the next page
                                for(int q = 0; q < section.LeftPages.Count; q++)
                                {
                                    if(section.LeftPages[q].Equals(pageToWriteTo))
                                    {
                                        left2 = true;
                                        index2 = q;
                                    }
                                }
                                for (int q = 0; q < section.RightPages.Count; q++)
                                {
                                    if (section.RightPages[q].Equals(pageToWriteTo))
                                    {
                                        left2 = false;
                                        index2 = q;
                                    }
                                }

                                //DestroyEntryManager(ref ManagerToAlter);
                                return PlaceEntriesHelper(ref entry, left2, index2, section);
                            }
                            int newIndex = AlteredEntry.textsAndImages[rip].image.index - 25;
                            if (newIndex >= 0)
                            {
                                AlteredEntry.textsAndImages[rip].image.index = newIndex;
                                break;
                            }
                            else
                            {//TODO: move the image to the previous block
                                AlteredEntry.textsAndImages[rip].image.index = 0;
                                break;
                            }
                        }
                        else { ImagesPassed++; }
                    }
                }

                AlterEntryManager(b, ref ManagerToAlter, AlteredEntry);
                b = DoesImageConflict(ManagerToAlter, section, pageToWriteTo);

                //oneEntry = NewEntryManager(AlteredEntry);
                //b = DoesImageConflict(oneEntry, section, pageToWriteTo);

                OldEntry = new JournalEntry(AlteredEntry);

                //if(b != oldB && b != -1)
                //{//fixed old b, now another one needs to be fixed
                //    continue;
                //}
                //else if(oldB == b)
                //{//revert to old
                    
                //}

                //if(b != -1)
                //{
                //    ManagerToAlter = new EntryManager(entry);  
                //}
                //change above
                //if we reach the point where is still conflicting after all possible rearrangements
                //revert to original entry, and call PlaceEntriesHelper on next page
            }
            if(ManagerToAlter != null)
            {
                DestroyEntryManager(ref entry);
                entry = ManagerToAlter;
            }
            //change entry to new entry that is found
            //if (oneEntry != null)
            //{
            //    DestroyEntryManager(ref entry);
             //   entry = oneEntry;
            //}
        }

        //float testEndTime = Time.realtimeSinceStartup;
        //float toDis = testEndTime - testTime;
        
        //Debug.Log(entry.entry.title + " TOOK " + toDis);
        

        return pageToWriteTo;
    }

    /// <summary>
    /// Alters a specific image block based on the new AlteredEntry
    /// </summary>
    /// <param name="b">Int of the image to alter</param>
    /// <param name="ToAlter">The EntryManager to alter</param>
    /// <param name="AlteredEntry">The altered JournalEntry to base off of</param>
    private void AlterEntryManager(int b, ref EntryManager ToAlter, JournalEntry AlteredEntry)
    {
        //Set the parent entry to the new altered entry
        //There will be a mismatch, now parent entry no longer points to an entry in JournalManager
        ToAlter.entry = AlteredEntry;

        //float start = Time.realtimeSinceStartup;

        //find the altered block with the image that is overflowing
        TextAndImage NewImageBlock = null;
        int imagesPassed = 0;
        for(int i = 0; i < AlteredEntry.textsAndImages.Count; i++)
        {
            if(AlteredEntry.textsAndImages[i].image != null)
            {
                if(imagesPassed == b)
                {
                    NewImageBlock = AlteredEntry.textsAndImages[i];
                    break;
                }
                imagesPassed++;
            }
        }
        
        //find the indices of the old blocks which have the image going over the current page
        List<int> indicesToChange = new List<int>();
        imagesPassed = 0;
        for(int i = 0; i < ToAlter.blocks.Count; i++)
        {
            if(ToAlter.blocks[i].GetType() == typeof(ImageBlockTracker))
            {//find the indices of the blocks to change
                if(imagesPassed == b)
                {
                    ImageBlockTracker imgTracker = (ImageBlockTracker)ToAlter.blocks[i];
                    if(imgTracker.State == ImageBlockTracker.StateOfBlocks.AboveAndBelow)
                    {
                        indicesToChange.Add(i - 1);
                        indicesToChange.Add(i);
                        indicesToChange.Add(i + 1);
                    }
                    else if(imgTracker.State == ImageBlockTracker.StateOfBlocks.NoneAround)
                    {
                        indicesToChange.Add(i);
                    }
                    else if(imgTracker.State == ImageBlockTracker.StateOfBlocks.OneAbove)
                    {
                        indicesToChange.Add(i - 1);
                        indicesToChange.Add(i);
                    }
                    else if(imgTracker.State == ImageBlockTracker.StateOfBlocks.OneBelow)
                    {
                        indicesToChange.Add(i);
                        indicesToChange.Add(i + 1);
                    }
                    break;
                }
                imagesPassed++;
            }
        }

        //Destroy the gameobject blocks of the old blocks
        foreach(int index in indicesToChange)
        {
            if(ToAlter.blocks[index].GetType() == typeof(ImageBlockTracker))
            {
                ImageBlockTracker im = (ImageBlockTracker)ToAlter.blocks[index];

                Destroy(im.PartnerBlock.block);
            }
            Destroy(ToAlter.blocks[index].block);
        }


        List<BlockTracker> newBlockTrackers = new List<BlockTracker>();

        //add the old blocks in, then create new image blocks based on the new altered entry
        int ind = 0;
        while(ind < ToAlter.blocks.Count)
        //for(int i = 0; i < ToAlter.blocks.Count; i++)
        {
            ToAlter.blocks[ind].parentEntry = AlteredEntry;
            if(ind == indicesToChange[0])
            {

                ImageBlockCreate(newBlockTrackers, NewImageBlock, AlteredEntry);

                int skip = indicesToChange.Count;
                
                ind += skip;
                continue;
            }

            newBlockTrackers.Add(ToAlter.blocks[ind]);
            ind++;
        }


        ToAlter.blocks = newBlockTrackers;
        //ToAlter.blocks.Clear();
        //foreach(BlockTracker blo in newBlockTrackers)
        //{
        //    ToAlter.blocks.Add(blo);
        //}


        //float end = Time.realtimeSinceStartup;

        //float tot = end - start;
        //Debug.Log("AlterEntriesManager FOR " + AlteredEntry.title + " TOOK " + tot);

    }

    /// <summary>
    /// Destroys all the blocks in an EntryManager and sets to null
    /// </summary>
    /// <param name="entry">Entry to destroy</param>
    private void DestroyEntryManager(ref EntryManager entry)
    {
        //foreach (BlockTracker boi in entry.blocks)
        for(int i = 0; i < entry.blocks.Count; i++)
        {
            BlockTracker boi = entry.blocks[i];
            Destroy(boi.block);
            if (boi.GetType() == typeof(ImageBlockTracker))
            {
                GameObject d = ((ImageBlockTracker)boi).PartnerBlock.block;
                Destroy(d);
            }
        }
        entry = null;
    }

    /// <summary>
    /// Checks if any of the image blocks in this entry will go other the edge of the page.
    /// </summary>
    /// <param name="entry">EntryManager of entry</param>
    /// <param name="section">Current section</param>
    /// <param name="page">Current page</param>
    /// <returns>Number of image block that conflicts, else -1</returns>
    private int DoesImageConflict(EntryManager entry, SectionTracker section, PageManager page)
    {
        //check if placing image on this page will conflict with image wrapping
        int m = 0;//m is the current number of images
        float currentPageHeight = page.heightFromTop;
        for(int v = 0; v < entry.blocks.Count; v++)
        { 
            currentPageHeight += entry.blocks[v].block.GetComponent<RectTransform>().rect.height + Settings.distBetweenBlocks;
            if(entry.blocks[v].GetType() == typeof(ImageBlockTracker))
            {
                if(currentPageHeight > pageTextMaxHeight)
                {
                    return m;
                }
                m++;
            }
            else
            {
                if(currentPageHeight > pageTextMaxHeight)
                {//normal wrap
                    currentPageHeight = currentPageHeight - pageTextMaxHeight;
                }
            }
        }

        return -1;

    }

    //TODO comment
    public void SectionDividerClick(int sec)
    {
        CurrentSection = sec;
        SetCurrentPage(0);
        
    }
    
    /// <summary>
    /// Places the blocks onto the specified page, from startingBlock to endingBlock. 
    /// Call this on an entry after checking you can place with CanPlaceEntryOnpage
    /// </summary>
    /// <param name="startingBlock">First block from ActualBlocksToDisplay to place, must be prior to endingBlock</param>
    /// <param name="endingBlock">Last block to place, must come after startingBlock in ActualBlocksToDisplay</param>
    /// <param name="ActualBlocksToDisplay">List containing the blocks to place</param>
    /// <param name="section">Current section of the journal we are working in</param>
    /// <param name="page">Page to start placing blocks onto</param>
    private void SetBlocksOnPage(BlockTracker startingBlock, BlockTracker endingBlock, List<BlockTracker> ActualBlocksToDisplay, SectionTracker section, PageManager page)
    {
        int startingIndex = ActualBlocksToDisplay.IndexOf(startingBlock);
        //do nothing if the starting or ending blocks are not in the list
        if (startingIndex < 0) return;
        if (ActualBlocksToDisplay.IndexOf(endingBlock) < 0) return;     

        BlockTracker OnePast;
        bool del = false;
        if (ActualBlocksToDisplay.IndexOf(endingBlock) == ActualBlocksToDisplay.Count - 1)
        {
            OnePast = this.gameObject.AddComponent<BlockTracker>();
            ActualBlocksToDisplay.Add(OnePast);
            del = true;
        }
        else
        {
            OnePast = ActualBlocksToDisplay[ActualBlocksToDisplay.IndexOf(endingBlock) + 1];
        }

        List<BlockTracker> queue = new List<BlockTracker>();
        for (int i = startingIndex; ActualBlocksToDisplay[i] != OnePast; i++)
        {
            queue.Add(ActualBlocksToDisplay[i]);
        }


        while (queue.Count > 0)
        {
            //current block to display
            BlockTracker currBlock = queue[0];
            
            RectTransform rt = currBlock.block.GetComponent<RectTransform>();

            if (!section.firstPlaced && page.page.Equals(section.LeftPages[0]) && page.heightFromTop == 0.0f)
            {//first block of first page
                if(!section.StartYSet)
                {
                    section.StartingY = rt.localPosition.y + (rt.rect.height / 2);
                    section.StartYSet = true;
                }

                //float dist = section.section == Section.TableOfContents ? distBetweenTOCBlocks : distBetweenBlocks;
                float dist = Settings.distBetweenBlocks;
                page.heightFromTop += rt.rect.height + dist;

                currBlock.block.transform.SetParent(page.page.transform, false);
                rt.rotation = Quaternion.Euler(0, 0, 0);//fixes text on right pages inheriting the page rotation

                SetChildrenActiveState(section.LatestPage.page, false);
                section.firstPlaced = true;
            }
            else
            {//filling in each page
                float prospectiveHeight = page.heightFromTop;//pass into NewPage
                prospectiveHeight += rt.rect.height;

                if (prospectiveHeight > pageTextMaxHeight)
                {
                    page.filled = true;
                    page = WrapToNextPage(currBlock, section, page);                   
                }
                else
                {
                    SetBlockPositionSpecial(currBlock, section, page);
                }
            }

            queue.RemoveAt(0);
        }

        if(del)
        {
            Destroy(OnePast);
            ActualBlocksToDisplay.Remove(OnePast);
        }

        SetHeirarchyOrder(section);
    }


    /// <summary>
    /// Places the block onto the page. Assumes the block will not wrap onto the next page
    /// </summary>
    /// <param name="block">The block to place</param>
    /// <param name="section">Current section</param>
    /// <param name="page">The page to place onto</param>
    private void SetBlockPositionSpecial(BlockTracker block, SectionTracker section, PageManager page)
    {
        RectTransform rt = block.block.GetComponent<RectTransform>();
        
        page.heightFromTop += rt.rect.height / 2;

        //if it is an image type, place the text block
        if(typeof(ImageBlockTracker) == block.GetType())
        {
            ImageBlockTracker img = (ImageBlockTracker)block;

            RectTransform pR = img.PartnerBlock.block.GetComponent<RectTransform>();
            float k = (rt.rect.height - pR.rect.height) / 2.0f;
            pR.localPosition = new Vector3(pR.localPosition.x, section.StartingY - page.heightFromTop + k, pR.localPosition.z);
            img.PartnerBlock.block.transform.SetParent(page.page.transform, false);
            pR.rotation = Quaternion.Euler(0, 0, 0);
        }

        rt.localPosition = new Vector3(rt.localPosition.x, section.StartingY - page.heightFromTop, rt.localPosition.z);
        //float dist = section.section == Section.TableOfContents ? distBetweenTOCBlocks : distBetweenBlocks;
        float dist = Settings.distBetweenBlocks;

        if (block.isBeforeImage)
        {
            dist /= 1.8f;
        }
        if (block.GetType() == typeof(ImageBlockTracker))
        {
            dist /= 1.4f;
        }

        page.heightFromTop += rt.rect.height / 2 + dist;


        block.block.transform.SetParent(page.page.transform, false);
        rt.rotation = Quaternion.Euler(0, 0, 0);//fixes text on right pages inheriting the page rotation      
    }


    /// <summary>
    /// Checks if the current entry can be placed on the page
    /// If it wraps to the next page, checks to make sure no entries already placed on the next page
    /// </summary>
    /// <param name="entry">Current entry to check</param>
    /// <param name="section">Section we are working in</param>
    /// <param name="page">Page to check against</param>
    /// <returns>PageAlreadyFilled if page is already filled, CanBePlaced if can place on the page or if will wrap onto next page with no issue, 
    /// NextPageOccupied if it will wrap onto next page and next page is occupied already, 
    /// and NextPageDoesntExist if it wraps onto next page but next page doesn't exist and need to create a new page</returns>
    private CanPlaceStatus CanPlaceEntryOnPage(EntryManager entry, SectionTracker section, PageManager page)
    {
        if (page.filled) return CanPlaceStatus.PageAlreadyFilled;
        float entryHeight = 0.0f;//total height of all the blocks in this entry

        foreach (BlockTracker blok in entry.blocks)
        {
            //float dist = section.section == Section.TableOfContents ? distBetweenTOCBlocks : distBetweenBlocks;
            float dist = Settings.distBetweenBlocks;
            if (blok.isBeforeImage)
            {
                dist /= 1.8f;
            }
            if(blok.GetType() == typeof(ImageBlockTracker))
            {
                dist /= 1.4f;
            }
            entryHeight += blok.block.GetComponent<RectTransform>().rect.height + dist;
        }

        //can fit inside current page
        //TODO: add e value maybe
        if (page.heightFromTop + entryHeight < pageTextMaxHeight) return CanPlaceStatus.CanBePlaced;

        else
        {

            PageManager nextPage = GetNextPage(section, page);
            if(nextPage == null)
            {
                return CanPlaceStatus.NextPageDoesntExist;//need to create new page to fit this entry
            }
            else
            {
                if(nextPage.heightFromTop > 0.0f)
                {
                    return CanPlaceStatus.NextPageOccupied;//next page exists but is already occupied
                }
                else
                {
                    return CanPlaceRecursiveHelper(entryHeight - (pageTextMaxHeight - page.heightFromTop), section, nextPage);
                    
                }
            }
        }
    }

    /// <summary>Recursive helper for CanPlaceEntryOnPage, for when an entry is really long and spans multiple pages</summary>
    private CanPlaceStatus CanPlaceRecursiveHelper(float heightLeft, SectionTracker section, PageManager page)
    {
        if(page.filled) return CanPlaceStatus.PageAlreadyFilled;

        if (heightLeft < pageTextMaxHeight) return CanPlaceStatus.CanBePlaced;

        else
        {
            PageManager nextPage = GetNextPage(section, page);
            if (nextPage == null)
            {
                return CanPlaceStatus.NextPageDoesntExist;//need to create new page to fit this entry
            }
            else
            {
                if (nextPage.heightFromTop > 0.0f)
                {
                    return CanPlaceStatus.NextPageOccupied;
                }
                
                return CanPlaceRecursiveHelper(heightLeft - pageTextMaxHeight, section, nextPage);
                
            }
        }
    }

    /// <summary>
    /// Returns the next page after the current page in the section, or null if page is last page in section
    /// </summary>
    /// <param name="section">Section to find next page in</param>
    /// <param name="page">Page that we are getting next page from</param>
    /// <returns>PageManager of next page, or null if page is last page</returns>
    private PageManager GetNextPage(SectionTracker section, PageManager page)
    {
        int index = section.LeftPages.IndexOf(page);
        if(index != -1)
        {//if page was a left page
            if (section.RightPages.Count - 1 >= index) return section.RightPages[index];
            else
            {
                return null;
            }
        }
        else
        {//page was a right page
            index = section.RightPages.IndexOf(page);
            if (section.LeftPages.Count - 1 >= index + 1) return section.LeftPages[index + 1];
            else
            {
                return null;
            }
        }
    }

    /// <summary>
    /// SetBlocksOnPage helper function, to help wrap a block onto the next. 
    /// Should only be called after it has been determined that the next page has nothing on it.
    /// </summary>
    /// <param name="block">Block that has to wrap</param>
    /// <param name="section">Current section of journal</param>
    /// <param name="page">Current page that we just filled</param>
    private PageManager WrapToNextPage(BlockTracker block, SectionTracker section, PageManager page)
    {
        BlockTracker fir = null;//this remains null if the block is a picture or if the entire block goes to next page
        BlockTracker sec = block;//will change to the secondhalf of the wrapped block if we have to split

        //if the block is a text block
        if(block.block.GetComponent<TextMeshProUGUI>() != null)
        {
            TextMeshProUGUI meshPro = block.block.GetComponent<TextMeshProUGUI>();

            string text = meshPro.text;

            bool split = false;//will change to true if we need to split current block to wrap

            //remove words from text, until it fits
            text = text.TrimEnd(' ');
            int lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);

            if(lio > 0)
            {
                text = text.Remove(lio);
            }

            float lastRectHeight = 1.0f;

            BlockTracker temp;
            /*if (section.section == Section.TableOfContents)
            {
                temp = block.block.name.Contains("Title") ? TextBoxCreate(TOCTitlePrefab, text, block.parentEntry) : TextBoxCreate(TOCTextPrefab, text, block.parentEntry);
                if (temp.GetComponent<Button>() != null)
                {
                    temp.GetComponent<Button>().onClick.AddListener(() => TextClick());
                }
            }*/
            //else
            //{
                temp = block.block.name.Contains("Title") ? TextBoxCreate(MenuManager.instance.TitlePrefab, text, block.parentEntry) : TextBoxCreate(MenuManager.instance.TextPrefab, text, block.parentEntry);
            //}

            while (true)
            {
                AddStringAndResize(temp, text);

                //temp.block.transform.SetParent(page.page.transform, false);THIS LINE FUCKED UP EVERYTHING, DONT KNOW WHY I HAVE THIS HERE
                RectTransform rt = temp.GetComponent<RectTransform>();

                if (lastRectHeight < 0.0f) lastRectHeight = rt.rect.height;

                if(lastRectHeight - rt.rect.height < minTextHeight)
                {//still on the old line
                    //Destroy(temp.block);
                    //Destroy(temp);
                    text = text.TrimEnd(' ');

                    lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);
                    if(lio < 0)
                    {
                        Destroy(temp.block);
                        break;
                    }
                    text = text.Remove(lio);
                    lastRectHeight = rt.rect.height;
                    continue;
                }

                //pageTextMaxHeight + e size to give a little bit of wiggle room
                if (page.heightFromTop + rt.rect.height < pageTextMaxHeight + minTextHeight)
                {//found where we need to spill over to next page               
                    string secondHalf = meshPro.text.Substring(text.Length + 1);
                    /*
                    if (section.section == Section.TableOfContents)
                    {
                        sec = block.block.name.Contains("Title") ? TextBoxCreate(TOCTitlePrefab, secondHalf, block.parentEntry) : TextBoxCreate(TOCTextPrefab, secondHalf, block.parentEntry);
                        if (sec.GetComponent<Button>() != null)
                        {
                            sec.GetComponent<Button>().onClick.AddListener(() => TextClick());
                        }
                    }
                    else
                    {
                    */
                        sec = block.name.Contains("Title") ? TextBoxCreate(MenuManager.instance.TitlePrefab, secondHalf, block.parentEntry) : TextBoxCreate(MenuManager.instance.TextPrefab, secondHalf, block.parentEntry);
                    //}

                    if(block.isBeforeImage)
                    {
                        sec.isBeforeImage = true;
                    }
                    
                    fir = temp;
                    split = true;

                    /*
                    if(section.section == Section.TableOfContents)
                    {
                        TOCMouseOverController firCont = fir.GetComponent<TOCMouseOverController>();
                        TOCMouseOverController secCont = sec.GetComponent<TOCMouseOverController>();
                        if (firCont != null && secCont != null)
                        {
                            firCont.PartnerController = secCont;
                            secCont.PartnerController = firCont;
                        }
                    }
                    */

                    //temp.SetParentEntry(block.parentEntry);
                    //sec.SetParentEntry(block.parentEntry);

                    Destroy(block.block);
                    Destroy(block);
                    
                    break;
                }                
                else
                {
                    //Destroy(temp.block);
                    //Destroy(temp);
                    text = text.TrimEnd(' ');

                    lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);
                    if (lio < 0)
                    {
                        Destroy(temp.block);
                        break;
                    }
                    text = text.Remove(lio);
                    lastRectHeight = rt.rect.height;
                }
            }

            if(split)
            {
                SetBlockPositionSpecial(fir, section, page);
            }

        }

        //restarting from top of next page
        PageManager nextPage = GetNextPage(section, page);
        //shouldn't need to worry that nextPage is not null as that stuff is checked before
        if (nextPage == null)
        {
            Debug.Log("ERROR: If this message is showing then Kevin boofed something up. Dumb Kevin. Or a picture is too big to fit on the pages.");
            return nextPage;
        }

        if(sec.GetComponent<RectTransform>().rect.height > pageTextMaxHeight)
        {//if a single block is so big that it takes up the entire next page
            nextPage.filled = true;
            return WrapToNextPage(sec, section, nextPage);
        }
        else
        {
            SetBlockPositionSpecial(sec, section, nextPage);
            return nextPage;
        }

    }


    /// <summary>
    /// Creates a new page at the end of the specified section
    /// </summary>
    /// <param name="section">Current section</param>
    protected void CreateBlankPage(SectionTracker section)
    {
        if (section.OnLeftPage)
        {//this is to make the right pages prefab the same as the next left page prefab
            section.currPrefab++;
        }

        section.LatestPage = new PageManager();

        section.LatestPage.page = section.OnLeftPage ? Instantiate(Settings.RightPagePrefabs[section.currPrefab % 3]) : Instantiate(Settings.LeftPagePrefabs[section.currPrefab % 3]);
        /*
        switch (section.section)
        {
            case Section.ParentSection:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(ParentRightPrefabs[section.currPrefab % 3]) : Instantiate(ParentLeftPrefabs[section.currPrefab % 3]);
                break;
            case Section.DreamSection:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(DreamRightPrefabs[section.currPrefab % 3]) : Instantiate(DreamLeftPrefabs[section.currPrefab % 3]);
                break;
            default:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(RightPagePrefabs[section.currPrefab % 3]) : Instantiate(LeftPagePrefabs[section.currPrefab % 3]);
                break;
        }
        */

        if (section.OnLeftPage)
        {
            section.RightPages.Add(section.LatestPage);
            //Test
            section.numOfPages++;
        }
        else
        {
            section.LeftPages.Add(section.LatestPage);
        }
        section.OnLeftPage = !section.OnLeftPage;
        //TESt, Changing to true
        section.LatestPage.page.transform.SetParent(section.holder.transform, false);
    }

    /// <summary>
    /// Sets all the children of the parent GameObject to active or inactive.
    /// Used specifically for setting the text on certain pages to display or hide.
    /// </summary>
    /// <param name="parent">Parent gameobject</param>
    /// <param name="active">True if setting to active, false is setting to inactive</param>
    private void SetChildrenActiveState(GameObject parent, bool active)
    {
        for (int i = 0; i < parent.transform.childCount; i++)
        {            
            parent.transform.GetChild(i).gameObject.SetActive(active);
        }
    }

    /// <summary>
    /// Takes a list of JournalEntry and converts them to UI GameObjects, stores them in an EntryManager. 
    /// Sets the GameObjects to inactive if they are hidden at the start.
    /// </summary>
    /// <param name="entries">List of JournalEntry to convert to UI GameObjects</param>
    /// <param name="manager">EntryManager to store the entry</param>
    private void FillAtStart(List<JournalEntry> entries, List<EntryManager> manager)
    {
        //bool isTOC = manager.Equals(Managers[(int)Section.TableOfContents]) ? true : false;
        for (int i = 0; i < entries.Count; i++)
        {
            JournalEntry entry = entries[i];

            EntryManager oneEntry = new EntryManager();
            oneEntry.entry = entries[i];

            //to keep track of where in the entry lists we are to maintain order
            int textListIndex = 0;
            //int imgListIndex = 0;

            for (int orderIndex = 0; orderIndex < entry.BlockOrder.Count; orderIndex++)
            {
                switch(entry.BlockOrder[orderIndex])
                {
                    case JournalEntry.BlockType.title:
                        JournalEntry titleParent;
                        /*
                        if (isTOC)
                        {
                            if (entry.TOCEntries.Count > 0)
                            {
                                titleParent = entry.TOCEntries[0];
                            }
                            else
                            {
                                Debug.Log("Table of contents title tracker error.");
                                titleParent = entry;
                            }
                        }
                        else
                        {*/
                            titleParent = entry;
                        //}
                        //BlockTracker title = isTOC ? TextBoxCreate(TOCTitlePrefab, entry.title, titleParent) : TextBoxCreate(TitlePrefab, entry.title, titleParent);
                        BlockTracker title = TextBoxCreate(MenuManager.instance.TitlePrefab, entry.title, titleParent);

                        oneEntry.blocks.Add(title);
                        title.transform.SetParent(Holder.transform, false);

                        //Added 9-27-2019, issue of holder objects appearing on top of the rest of the objects
                        //title.block.SetActive(false);
                        break;
                    case JournalEntry.BlockType.maintext:
                        if(entry.textsAndImages[textListIndex].image != null)
                        {//there are images that we need to process
                            //convert one text and images into up to 3 blocks
                            ImageBlockCreate(oneEntry.blocks, entry.textsAndImages[textListIndex], entry);                           
                        }
                        else
                        {
                            JournalEntry toAdd;
                            /*
                            if (isTOC)
                            {
                                if (entry.TOCEntries.Count > textListIndex)
                                {
                                    toAdd = entry.TOCEntries[textListIndex];
                                }
                                else
                                {
                                    Debug.Log("Table of contents entry tracker error.");
                                    toAdd = entry;
                                }
                            }
                            else
                            {
                            */
                                toAdd = entry;
                            //}
                            //TODO: MIGHT NEED TO LOOK HERE AGAIN
                            //BlockTracker text = isTOC ? TextBoxCreate(TOCTextPrefab, entry.textsAndImages[textListIndex].text, toAdd) :
                            //    TextBoxCreate(TextPrefab, entry.textsAndImages[textListIndex].text, toAdd);
                            BlockTracker text = TextBoxCreate(MenuManager.instance.TextPrefab, entry.textsAndImages[textListIndex].text, toAdd);


                            oneEntry.blocks.Add(text);
                            text.transform.SetParent(Holder.transform, false);

                            //Added 9-27-2019, issue of holder objects appearing on top of the rest of the objects
                            //text.block.SetActive(false);
                        }
                        textListIndex++;

                        break;

                }              

            }

            manager.Add(oneEntry);

        }

    }

   
    /// <summary>
    /// Creates a text box UI object out of the specified prefab and string
    /// </summary>
    /// <param name="prefab">Prefab to use</param>
    /// <param name="entry">String in the text box</param>
    /// <param name="parentEntry">Parent JournalEntry</param>
    /// <returns>Text Box UI GameObject</returns>
    private BlockTracker TextBoxCreate(GameObject prefab, string entry, JournalEntry parentEntry)
    {
        GameObject ret = Instantiate(prefab);

        /*
        if(ret.GetComponent<Button>() != null)
        {
            ret.GetComponent<Button>().onClick.AddListener(() => TextClick());
        }
        */
        
        TextMeshProUGUI meshPro = ret.GetComponent<TextMeshProUGUI>();//get the text from the prefab
        meshPro.text = entry;

        //blockmanager
        BlockTracker tracker = ret.GetComponent<BlockTracker>();
        
        tracker.SetParentEntry(parentEntry);

        //setting the size of the UI textbox
        RectTransform rt = ret.GetComponent<RectTransform>();
        rt.sizeDelta = new Vector2(pageTextMaxWidth, rt.sizeDelta.y);//setting the max width, so stays on page

        //TextGenerator textGen = new TextGenerator();
        meshPro.ForceMeshUpdate();
        Bounds bounds = meshPro.bounds;
        float prefHeight = bounds.extents.y;
        //now we add the prefheight to the x of the object so that they all start at the same place
        //could probably just change the anchors on the prefabs tbh
        rt.sizeDelta = new Vector2(rt.sizeDelta.x, prefHeight * 2.0f);
        rt.position = new Vector3(rt.position.x, rt.position.y - prefHeight, rt.position.z);
        //THIS IS WORKING NOW



        return tracker;
    }

    /// <summary>
    /// Overload for TextBoxCreate, can specify the width of the generated text box
    /// </summary>
    /// <param name="prefab">Prefab to use</param>
    /// <param name="entry">String in the text box</param>
    /// <param name="parentEntry">Parent JournalEntry</param>
    /// <param name="boxWidth">Width of the box to generate</param>
    /// <returns>Text Box UI GameObject with specified width</returns>
    private BlockTracker TextBoxCreate(GameObject prefab, string entry, JournalEntry parentEntry, float boxWidth)
    {
        GameObject ret = Instantiate(prefab);

        /*
        if (ret.GetComponent<Button>() != null)
        {
            ret.GetComponent<Button>().onClick.AddListener(() => TextClick());
        }
        */


        TextMeshProUGUI meshPro = ret.GetComponent<TextMeshProUGUI>();//get the text from the prefab
        meshPro.text = entry;

        //blockmanager
        BlockTracker tracker = ret.GetComponent<BlockTracker>();

        tracker.SetParentEntry(parentEntry);

        //setting the size of the UI textbox
        RectTransform rt = ret.GetComponent<RectTransform>();

        //rt.pivot = new Vector2(1, 0.5f);
        rt.sizeDelta = new Vector2(boxWidth, rt.sizeDelta.y);//setting the max width, so stays on page

        //TextGenerator textGen = new TextGenerator();
        meshPro.ForceMeshUpdate();
        Bounds bounds = meshPro.bounds;
        float prefHeight = bounds.extents.y;
        //now we add the prefheight to the x of the object so that they all start at the same place
        //could probably just change the anchors on the prefabs tbh
        rt.sizeDelta = new Vector2(rt.sizeDelta.x, prefHeight * 2.0f);
        rt.position = new Vector3(rt.position.x, rt.position.y - prefHeight, rt.position.z);
        //THIS IS WORKING NOW



        return tracker;
    }

    /// <summary>
    /// Generates the ImageBlockTracker and all the surrounding text blocks for a TextAndImage with image.
    /// Automatically adds to the entry after creating
    /// </summary>
    /// <param name="entry">The EntryManager to add to</param>
    /// <param name="textAndImage">TextAndImage block with the image</param>
    /// <param name="parentEntry">Parent JournalEntry</param>
    private void ImageBlockCreate(/*EntryManager entry*/List<BlockTracker> blocks, TextAndImage textAndImage, JournalEntry parentEntry)
    {

        Img img = textAndImage.image;
        int tempIndex = img.index;
        ImageBlockTracker imageBlock = ImgBoxCreate(MenuManager.instance.ImgPrefab, img.img, parentEntry);


        string tempText = textAndImage.text;
        if (tempIndex != 0)
        {//create the text box that will go on top of the image block
            string topBoxText = tempText.Substring(0, tempIndex);
            //tempText.Replace(topBoxText, "");

            //TODO: MAKE SURE THE BLOCK IS WRAPPED
            BlockTracker topBlock = TextBoxCreate(MenuManager.instance.TextPrefab, topBoxText, parentEntry);

            float lastHeight = topBlock.GetComponent<RectTransform>().rect.height;
            while (true)
            {
                //should be adding only at spaces here
                int hold = tempIndex;
                tempIndex = tempIndex < tempText.Length - 1 ? tempText.IndexOf(" ", tempIndex + 1) : tempText.IndexOf(" ", tempIndex);
                if(tempIndex == -1)
                {
                    tempIndex = hold;
                    break;
                }
                string newText = tempText.Substring(0, tempIndex);
                AddStringAndResize(topBlock, newText);
                //BlockTracker newTracker = TextBoxCreate(TextPrefab, newText, parentEntry);
                if (topBlock.GetComponent<RectTransform>().rect.height - lastHeight > minTextHeight)
                {
                    tempIndex = hold;
                    string oldText = tempText.Substring(0, tempIndex);
                    AddStringAndResize(topBlock, oldText);
                    //Destroy(newTracker.block);
                    break;
                }

            }          

            //now that the top block is generated, we add it

            topBlock.block.transform.SetParent(Holder.transform, false);
            topBlock.isBeforeImage = true;
            topBlock.isPartOfImage = true;
            /*entry.*/blocks.Add(topBlock);
            imageBlock.SetStateOfBlocks(ImageBlockTracker.StateOfBlocks.OneAbove);
        }



        //now we create the img partner block of text
        string partnerText = "";
        //if(i == textAndImage.images.Count - 1)
        //{//get the rest of temp
        partnerText = tempText.Substring(tempIndex);

        string partnerTextOriginal = string.Copy(partnerText);
        

        float width = pageTextMaxWidth - imageBlock.GetComponent<RectTransform>().rect.width;
        string temp1 = partnerText.Trim();
        BlockTracker partnerTracker = TextBoxCreate(MenuManager.instance.TextPrefab, temp1, parentEntry, width);

        RectTransform partnerRect = partnerTracker.block.GetComponent<RectTransform>();
        RectTransform imageRect = imageBlock.block.GetComponent<RectTransform>();

        //cuts the remaining text in half repeatedly to speed up loadtime
        //AddStringAndResize takes around 0.001 sec, the less we call it the better
        string partnerTextCopy = partnerText;
        bool needToResize = false;
        while(partnerRect.rect.height > imageRect.rect.height + (minTextHeight * 0.3f))
        {
            if (partnerText.Length == 0) break;
            needToResize = true;
            partnerTextCopy = partnerText;
            //cut in half
            partnerText = partnerText.Substring(0, partnerText.Length / 2);
            AddStringAndResize(partnerTracker, partnerText);
        }
        if(needToResize)
        {
            partnerText = partnerTextCopy;
            AddStringAndResize(partnerTracker, partnerText);
        }
        
        //

        while (partnerRect.rect.height > imageRect.rect.height + (minTextHeight * 0.3f))
        {
            partnerText = partnerText.TrimEnd(' ');

            int lio = partnerText.LastIndexOf(' ') > partnerText.LastIndexOf(System.Environment.NewLine) ? partnerText.LastIndexOf(' ') : partnerText.LastIndexOf(System.Environment.NewLine);
            if (lio < 0)
            {
                break;
            }
            partnerText = partnerText.Remove(lio);
            //Destroy(partnerTracker.block);
            string temp2 = partnerText.Trim();
            //float TIME = Time.realtimeSinceStartup;
            AddStringAndResize(partnerTracker, temp2);
            
        }
        imageBlock.SetPartner(partnerTracker);




        partnerTextOriginal = partnerTextOriginal.Remove(0, partnerText.Length);//bottom text
        RectTransform iR = imageBlock.block.GetComponent<RectTransform>();
        RectTransform pR = partnerTracker.block.GetComponent<RectTransform>();

        //making image same height as partner
        float diff = iR.rect.height - pR.rect.height;
        float percentDiff = diff / iR.rect.height;
        float widthDiff = percentDiff * iR.rect.width;

        // padding to give a little room between the text and the image
        iR.localPosition = new Vector3(-(Settings.ImagePadding) + iR.localPosition.x - pR.rect.width / 2, iR.localPosition.y, iR.localPosition.z);
        pR.localPosition = new Vector3(iR.rect.width / 2 + pR.localPosition.x, pR.localPosition.y, pR.localPosition.z);


        if (partnerTextOriginal.Length > 0)
        {
            iR.sizeDelta = new Vector2(iR.rect.width - widthDiff, iR.rect.height - diff);
            //iR.localPosition = new Vector3(iR.localPosition.x, iR.localPosition.y + diff / 2, iR.localPosition.z);
            //All y position is done when the item is actuall placed on the page
        }


        imageBlock.block.transform.SetParent(Holder.transform, false);
        partnerTracker.block.transform.SetParent(Holder.transform, false);
        partnerTracker.isPartOfImage = true;
        imageBlock.PartnerBlock = partnerTracker;
        /*entry.*/blocks.Add(imageBlock);
        

        //create the stuff at the end
        if (partnerTextOriginal.Length > 0)
        {
            string temp3 = partnerTextOriginal.Trim();
            BlockTracker bottomBlock = TextBoxCreate(MenuManager.instance.TextPrefab, temp3, parentEntry);
            bottomBlock.isPartOfImage = true;
            bottomBlock.block.transform.SetParent(Holder.transform, false);
            /*entry.*/blocks.Add(bottomBlock);
            ImageBlockTracker.StateOfBlocks s = imageBlock.State == ImageBlockTracker.StateOfBlocks.OneAbove ?
                ImageBlockTracker.StateOfBlocks.AboveAndBelow : ImageBlockTracker.StateOfBlocks.OneBelow;
            imageBlock.SetStateOfBlocks(s);
        }



    }

    /// <summary>
    /// Recreates the BlockTracker's block gameobject with a new string and resizes it to best fit
    /// </summary>
    /// <param name="block">BlockTracker to change</param>
    /// <param name="newString">New string for the block to display</param>
    private void AddStringAndResize(BlockTracker block, string newString)
    {
        RectTransform rt = block.block.GetComponent<RectTransform>();
        TextMeshProUGUI meshPro = block.block.GetComponent<TextMeshProUGUI>();
        
        if (meshPro != null)
        {
            if(string.Compare(meshPro.text, newString) == 0)
            {
                return;
            }
            meshPro.text = newString;
            meshPro.ForceMeshUpdate();
            Bounds bounds = meshPro.bounds;
            float prefHeight = bounds.extents.y;
            //now we add the prefheight to the x of the object so that they all start at the same place
            //could probably just change the anchors on the prefabs tbh
            rt.sizeDelta = new Vector2(rt.sizeDelta.x, prefHeight * 2.0f);
            //TODO idk if we need this line
            //rt.position = new Vector3(rt.position.x, rt.position.y - prefHeight, rt.position.z);
        }
        
    }

    /// <summary>
    /// Creates an image box UI object out of the specified prefab and texture
    /// </summary>
    /// <param name="prefab">Prefab to use</param>
    /// <param name="texture">Image to use</param>
    /// <returns>Image Box UI GameObject</returns>
    private ImageBlockTracker ImgBoxCreate(GameObject prefab, Texture2D texture, JournalEntry parentEntry)
    {
        GameObject ret = Instantiate(prefab);

        RawImage img = ret.GetComponent<RawImage>();
        img.texture = texture;
        float aspectRatio = (float)texture.width / (float)texture.height;

        ImageBlockTracker tracker = ret.GetComponent<ImageBlockTracker>();


        //setting size
        RectTransform rt = ret.GetComponent<RectTransform>();
        float maxWidth = pageTextMaxWidth * Settings.ImageWidthPercent;//0.5f;
        float maxHeight = pageTextMaxHeight * Settings.ImageHeightPercent;//0.5f;
        rt.sizeDelta = aspectRatio > 1.0f ? new Vector2(maxWidth, maxWidth / aspectRatio) : new Vector2(maxHeight * aspectRatio, maxHeight);
        rt.position = new Vector3(rt.position.x, rt.position.y - rt.rect.height/2, rt.position.z);

        tracker.SetParentEntry(parentEntry);

        return tracker;
    }

    /// <summary>
    /// Sets the heirarchy of the pages so they display in the correct order
    /// </summary>
    /// <param name="section">Section to set page heirarchy in</param>
    private void SetHeirarchyOrder(SectionTracker section)
    {
        for (int i = section.RightPages.Count - 1; i >= 0; i--)
        {
            section.RightPages[i].page.gameObject.GetComponent<RectTransform>().SetAsLastSibling();
        }
    }
    #endregion
}

#region Old Code
/*
 private bool AddEntryHelper(JournalEntry entry, List<EntryManager> entryManager, List<GameObject> blocks, Section section)
 {
     for(int i = 0; i < entryManager.Count; i++)
     {
         if(entryManager[i].entry.Equals(entry))
         {
             foreach(GameObject obj in entryManager[i].blocks)
             {
                 obj.SetActive(true);
                 SetBlocksOnPage(obj, obj, blocks, SectionManager[(int)section]);
                 //if this block wraps to next page, then the manager will be inaccurate
                 //but doesnt matter because we no longer need to manage after placing on the page
                 //since we are not implementing remove functionality
             }


             SetCurrentPage(CurrentPages);
             Holder.SetActive(false);
             return true;
         }
     }
     return false;
 }

//places the block on the page
private void SetBlockPosition(GameObject block, SectionTracker section)
{
    RectTransform rt = block.GetComponent<RectTransform>();
    section.LatestPage.heightFromTop += rt.rect.height / 2;

    /*since the size of the text box is determined by the text, letters like g, y, p, that dangle below
    //makes the box bigger than letters like e, o, c, etc. this is accounting for that
    float fineTune = DoesPrevDangle(block, indexOfBlock);
    if (fineTune < 0.0f)
    {
        fineTune = 0.0f;
    }
    //Not really worth the effort, can do it later if it is worth it and doesnt match loading longer

    rt.localPosition = new Vector3(rt.localPosition.x, section.StartingY - section.LatestPage.heightFromTop, rt.localPosition.z);
    section.LatestPage.heightFromTop += rt.rect.height / 2 + distBetweenBlocks;

    block.transform.SetParent(section.LatestPage.page.transform, false);
    rt.rotation = Quaternion.Euler(0, 0, 0);//fixes text on right pages inheriting the page rotation

    SetChildrenActiveState(section.LatestPage.page, false);
}

private bool DoesPrevDangle(GameObject block, int indexOfBlock)
{//returns how much dangle there is, or -1 if none
     if (indexOfBlock == 0)
     {
         return -1.0f;
     }
     if (block.GetComponent<TextMeshProUGUI>() != null)
     {
         return -1.0f;
     }

     GameObject prev = ActualBlocksToDisplay[indexOfBlock - 1];
     float height = prev.GetComponent<RectTransform>().rect.height;

     TextMeshProUGUI star = prev.GetComponent<TextMeshProUGUI>();
     string text = star.text;

     char[] danglers = ""

     while (true)
     {
         GameObject temp = star.isUsingBold ? TextBoxCreate(TitlePrefab, text) : TextBoxCreate(TextPrefab, text);
         float reducedHeight = temp.GetComponent<RectTransform>().rect.height;

         text = text.TrimEnd(' ');

         string toAnalyze = text.Substring(text.LastIndexOf(' ') + 1);
         if(toAnalyze.Contains)

         if (text.LastIndexOf(' ') < 0)
         {
             return -1.0f;
         }
         text = text.Remove(text.LastIndexOf(' '));
     }

}
    
    //when a new page needs to be created
    private void NewPage(GameObject block, SectionTracker section)
    {//return the new height
        GameObject fir = null;
        GameObject sec = block;

        if (block.GetComponent<TextMeshProUGUI>() != null)
        {
            TextMeshProUGUI meshPro = block.GetComponent<TextMeshProUGUI>();

            string text = meshPro.text;


            bool split = false;//true if current block is split
            //remove words from text, until it fits
            text = text.TrimEnd(' ');
            int lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);

            if (lio > 0)
            {
                text = text.Remove(lio);
            }

            float lastRectHeight = -1.0f;
            while (true)
            {
                GameObject temp = block.name.Contains("Title") ? TextBoxCreate(TitlePrefab, text) : TextBoxCreate(TextPrefab, text);
                temp.transform.SetParent(section.LatestPage.page.transform, false);
                RectTransform rt = temp.GetComponent<RectTransform>();

                if (lastRectHeight < 0.0f)
                {
                    lastRectHeight = rt.rect.height;
                }

                if (lastRectHeight - rt.rect.height < minTextHeight)
                {//still on the old line
                    Destroy(temp);
                    text = text.TrimEnd(' ');

                    lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);
                    if (lio < 0)
                    {
                        break;
                    }
                    text = text.Remove(lio);
                    lastRectHeight = rt.rect.height;
                    continue;
                }                

                //pageTextMaxHeight + e size to give a little bit of wiggle room
                if (section.LatestPage.heightFromTop + rt.rect.height < pageTextMaxHeight + minTextHeight)
                {//found where we need to spill over to next page
                    //string firstHalf = text;
                    string secondHalf = meshPro.text.Substring(text.Length + 1);


                    sec = block.name.Contains("Title") ? TextBoxCreate(TitlePrefab, secondHalf) : TextBoxCreate(TextPrefab, secondHalf);

                    Destroy(block);

                    //Destroy(ActualBlocksToDisplay[actualBlocksIndex]);
                    //ActualBlocksToDisplay[actualBlocksIndex] = temp;

                    //string firstHalf = temp.GetComponent<TextMeshProUGUI>().text;

                    //ActualBlocksToDisplay.Insert(actualBlocksIndex + 1, second);
                    fir = temp;
                    split = true;
                    break;
                }
                else
                {
                    Destroy(temp);
                    text = text.TrimEnd(' ');

                    lio = text.LastIndexOf(' ') > text.LastIndexOf(System.Environment.NewLine) ? text.LastIndexOf(' ') : text.LastIndexOf(System.Environment.NewLine);
                    if (lio < 0)
                    {
                        break;
                    }
                    text = text.Remove(lio);
                    lastRectHeight = rt.rect.height;
                }
            }

            if (split)
            {//print the first half on the first page
                //GameObject fir = ActualBlocksToDisplay[actualBlocksIndex];
                //test
                //string test = fir.GetComponent<TextMeshProUGUI>().text;

                SetBlockPosition(fir, section);
                /*ActualBlocksToDisplay[actualBlocksIndex].transform.SetParent(page.transform, false);
                RectTransform rt = ActualBlocksToDisplay[actualBlocksIndex].GetComponent<RectTransform>();
                rt.rotation = Quaternion.Euler(0, 0, 0);//fixes text on right pages inheriting the page rotation
                SetChildrenActiveState(page, false);
                //actualBlocksIndex++;

            }



        }
        //
        if (section.OnLeftPage)
        {//this is to make the right pages prefab the same as the next left page prefab
            section.currPrefab++;
        }
        //section.page = section.OnLeftPage ? Instantiate(RightPagePrefabs[section.currPrefab % 3]) : Instantiate(LeftPagePrefabs[section.currPrefab % 3]);

        section.LatestPage.filled = true;
        section.LatestPage = new PageManager();

        switch (section.section)
        {
            case Section.ParentSection:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(ParentRightPrefabs[section.currPrefab % 3]) : Instantiate(ParentLeftPrefabs[section.currPrefab % 3]);
                break;
            case Section.DreamSection:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(DreamRightPrefabs[section.currPrefab % 3]) : Instantiate(DreamLeftPrefabs[section.currPrefab % 3]);
                break;
            default:
                section.LatestPage.page = section.OnLeftPage ? Instantiate(RightPagePrefabs[section.currPrefab % 3]) : Instantiate(LeftPagePrefabs[section.currPrefab % 3]);
                break;
        }

        if (section.OnLeftPage)
        {
            section.RightPages.Add(section.LatestPage);
        }
        else
        {
            section.LeftPages.Add(section.LatestPage);
        }
        section.OnLeftPage = !section.OnLeftPage;
        section.LatestPage.page.transform.SetParent(section.holder.transform, false);
        //


        //restarting from the top of next page
        section.LatestPage.heightFromTop = 0.0f;
        //GameObject sec = ActualBlocksToDisplay[actualBlocksIndex];
        //string test2 = sec.GetComponent<TextMeshProUGUI>().text;
        if (sec.GetComponent<RectTransform>().rect.height > pageTextMaxHeight)
        {
            NewPage(sec, section);
        }
        else
        {
            SetBlockPosition(sec, section);
        }


        //rect.localPosition = new Vector3(rect.localPosition.x, StartingY - rect.rect.height / 2, rect.localPosition.z);
        //heightFromTop += rect.rect.height + distBetweenBlocks;

    }
 public void ShowOrHideEntry(JournalEntry entry, bool show)
{
    if(HideEntryHelper(show, entry, PlayerBlockManager, PlayerBlocksToDisplay))
    {
        return;
    }
    if(HideEntryHelper(show, entry, ParentBlockManager, ParentBlocksToDisplay))
    {
        return;
    }
    if(HideEntryHelper(show, entry, EnvironmentBlockManager, EnvironmentBlocksToDisplay))
    {
        return;
    }

    HideEntryHelper(show, entry, DreamBlockManager, DreamBlocksToDisplay);
}
private bool HideEntryHelper(bool show, JournalEntry entryToHide, List<EntryManager> manager, List<GameObject> DisplayList)
{
    for(int i = 0; i < manager.Count; i++)
    {
        if(manager[i].entry.Equals(entryToHide))
        {
            for(int m = 0; m < manager[i].blocks.Count; m++)
            {//go find all the ones
                if(show)
                {
                    DisplayList[manager[i].indices[m]].gameObject.SetActive(true);
                }
                else
                {
                    DisplayList[manager[i].indices[m]].gameObject.SetActive(false);
                }
            }

            return true;
        }
    }

    return false;
}

/// <summary>
/// Creates a new EntryManager from the provided entry. Used for image wrapping when images overflow current page
/// </summary>
/// <param name="entry">JournalEntry to generate new EntryManager from</param>
/// <returns>EntryManager from provided JournalEntry</returns>
//private EntryManager NewEntryManager(JournalEntry entry)
//{
//    EntryManager oneEntry = new EntryManager();
//    oneEntry.entry = entry;
//    int textListIndex = 0;
//    for (int orderIndex = 0; orderIndex < entry.BlockOrder.Count; orderIndex++)
//    {
//        switch (entry.BlockOrder[orderIndex])
//        {
//            case JournalEntry.BlockType.title:
//                JournalEntry titleParent;

//                titleParent = entry;

//                BlockTracker title = TextBoxCreate(TitlePrefab, entry.title, titleParent);                   

//                oneEntry.blocks.Add(title);
//                title.transform.SetParent(Holder.transform, false);
//                break;
//            case JournalEntry.BlockType.maintext:
//                if (entry.textsAndImages[textListIndex].image != null)
//                {//there are images that we need to process
//                 //convert one text and images into up to 3 blocks
//                    ImageBlockCreate(oneEntry.blocks, entry.textsAndImages[textListIndex], entry);
//                }
//                else
//                {
//                    JournalEntry toAdd;

//                    toAdd = entry;

//                    //TODO: MIGHT NEED TO LOOK HERE AGAIN
//                    BlockTracker text = TextBoxCreate(TextPrefab, entry.textsAndImages[textListIndex].text, toAdd);
                            
//                    oneEntry.blocks.Add(text);
//                    text.transform.SetParent(Holder.transform, false);

//                }
//                textListIndex++;

//                break;

//        }

//    }
//    return oneEntry;
//}
*/


        /* Table of Contents is dead
    /// <summary>
    /// Updates TableOfContentsEntries in JournalManager with current entries placed in the journal. 
    /// Call at the end of PlaceEntriesInJournal.
    /// </summary>
    /// <param name="section">The section to update</param>
    private void UpdateTableOfContents(SectionTracker section)
    {//this effectively takes the place of fillatstart for the Table of Contents
     //instead of parsing the entry to fill the blocks in entrymanager like fill at start
     //we just fill the blocks here based on the titles showing on page

        journal.TableOfContentsEntries[(int)section.section - 1].BlockOrder.Clear();
        journal.TableOfContentsEntries[(int)section.section - 1].textsAndImages.Clear();
        journal.TableOfContentsEntries[(int)section.section - 1].TOCEntries.Clear();
        journal.TableOfContentsEntries[(int)section.section - 1].BlockOrder.Add(JournalEntry.BlockType.title);

        PageManager currPage = section.LeftPages[0];

        do
        {
            foreach (Transform child in currPage.page.transform)
            {
                if (child.gameObject.name.Contains("Title"))
                {
                    BlockTracker tracker = child.gameObject.GetComponent<BlockTracker>();     

                    List<JournalEntry> temp = journal.TableOfContentsEntries[(int)section.section - 1].TOCEntries;
                    if (temp.Count > 0)
                    {
                        if (temp[temp.Count - 1].Equals(tracker.parentEntry)) continue;
                    }
                    TextAndImage t = new TextAndImage();

                    
                    //Debug.Log(tracker.block.GetComponent<TextMeshProUGUI>().text);
                    t.text = tracker.parentEntry.title;
                    journal.TableOfContentsEntries[(int)section.section - 1].textsAndImages.Add(t);
                    journal.TableOfContentsEntries[(int)section.section - 1].BlockOrder.Add(JournalEntry.BlockType.maintext);
                    temp.Add(tracker.parentEntry);

                }
            }
            currPage = GetNextPage(section, currPage);
        } while (currPage != null);
    }
    */
    /*
    /// <summary>
    /// Called on click from the table of contents entries.
    /// </summary>
    public void TextClick()
    {
        //TESTING, DELETE LATER
        JournalButtonScript.instance.TurnOnAndOffGlow();


        BlockTracker b = EventSystem.current.currentSelectedGameObject.GetComponent<BlockTracker>();
        foreach(SectionTracker section in SectionManager)
        {
            if(section.section == Section.TableOfContents)
            {
                continue;
            }
            if (TextClickHelper(section, b)) return;
           
        }
        
    }
    

    /// <summary>
    /// Helper function that finds the page to go to and calls SetCurrentPage.
    /// </summary>
    /// <param name="section">Section to look through</param>
    /// <param name="pages">Pages to look through, either left or right pages</param>
    /// <param name="b">The block tracker to match with</param>
    /// <param name="isLeftPages">True if pages is left pages</param>
    /// <returns>True if entry is found and page is set</returns>
    private bool TextClickHelper(SectionTracker section, BlockTracker b)
    {
        if (section.LeftPages.Count == 0) return false;
        //look through each page
        PageManager page = section.LeftPages[0];
        bool isLeftPages = true;
        while(page != null)
        {
            //for each block on the pages
            foreach (Transform child in page.page.transform)
            {
                if (!child.gameObject.name.Contains("Title")) continue;
                //if it has a blocktracker
                if (child.gameObject.GetComponent<BlockTracker>() != null)
                {
                    //if they have the same parent entry
                    if (child.gameObject.GetComponent<BlockTracker>().parentEntry.Equals(b.parentEntry))
                    {
                        int pageNum = isLeftPages ? section.LeftPages.IndexOf(page) : section.RightPages.IndexOf(page);

                        int numTemp = pageNum;
                        while (CurrentSection != section.section)
                        {
                            SetCurrentPage(++numTemp);
                        }
                        SetCurrentPage(pageNum);
                        //resetting text back to normal size
                        EventSystem.current.currentSelectedGameObject.GetComponent<TOCMouseOverController>().OnPointerExit(null);

                        return true;
                    }
                }

            }
            page = GetNextPage(section, page);
            isLeftPages = !isLeftPages;
        }
        return false;
    }
    

    /// <summary>
    /// Used to reset a section of the journal. Mainly for table of contents when updating it.
    /// </summary>
    /// <param name="section">Section to reset</param>
    private void ResetSection(SectionTracker section)
    {
        TableOfContentsManager = new List<EntryManager>();
        Managers[(int)section.section] = TableOfContentsManager;

        SectionManager[(int)section.section] = new SectionTracker(Section.TableOfContents, TableOfContentsHolder);
    }
    */
#endregion
